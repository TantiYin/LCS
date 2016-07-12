#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <stack>
using namespace std;

/*最长公共字串问题(longest common string)
可分为三种：
1.公共字串的元素必须相邻
2.公共字串的元素可以不相邻
3.求多个字符串而不是两个字符串的公共字串*/

/*1.实际就是求s1与s2所有前缀的最长公共后缀，最后返回所有公共后缀中最长的即可*/
string LCS1(string s1, string s2)
{
	vector<vector<string>> lcsuffix(s1.length() + 1, vector<string>(s2.length() + 1));
	for (int i = 0; i < s1.length() + 1; ++i)
	{
		lcsuffix[i][0] = "";
	}
	for (int i = 0; i < s2.length() + 1; ++i)
	{
		lcsuffix[0][i] = "";
	}
	for (int i = 1; i < s1.length() + 1; ++i)
	{
		for (int j = 1; j < s2.length() + 1; ++j)
		{
			if (s1[i - 1] == s2[j - 1])
			{
				lcsuffix[i][j] = lcsuffix[i - 1][j - 1] + s1[i - 1];
			}
			else
			{
				lcsuffix[i][j] = "";
			}
		}
	}
	int mi = 0, mj = 0;
	int mlen = 0;
	for (int i = 1; i < s1.length() - 1; ++i)
	{
		for (int j = 1; j < s2.length() - 1; ++j)
		{
			if (lcsuffix[i][j].length() > mlen)
			{
				mi = i;
				mj = j;
				mlen = lcsuffix[i][j].length();
			}
		}
	}
	return lcsuffix[mi][mj];
}

/*2.*/
string LCS2(string s1, string s2)
{
	vector<vector<string>> lcs(s1.length() + 1, vector<string>(s2.length() + 1));

	for (int i = 0; i < s1.length() + 1; ++i)
	{
		lcs[i][0] = "";
	}
	for (int i = 0; i < s2.length() + 1; ++i)
	{
		lcs[0][i] = "";
	}
	for (int i = 1; i < s1.length() + 1; ++i)
	{
		for (int j = 1; j < s2.length() + 1; ++j)
		{
			if (s1[i - 1] == s2[j - 1])
			{
				lcs[i][j] = lcs[i - 1][j - 1] + s1[i - 1];
			}
			else
			{
				lcs[i][j] = lcs[i - 1][j].length() > lcs[i][j - 1].length()? lcs[i - 1][j]: lcs[i][j - 1];
			}
		}
	}
	return lcs[s1.length()][s2.length()];
}

/*3.广义后缀树(generalized suffix tree)算法
就是把给定的n个字符串的所有后缀建成一棵树，这棵树有如下特点：
1.树的每个节点是一个字符串，树根是空字符串
2.任意一个后缀字串都可以由一条从根出发的路径表示
3.特别注意任意一个字串都可以看做某一个后缀的前缀，既然每一个后缀都可以甴一条从根开始的路径表示，
那么我们可以从根节点开始一个字符一个字符地跟踪这条路径从而得到任意一个字串
4.为了满足查找公共字串的需求，每个节点还应该有从属于哪个字符串的信息*/
struct Node
{
	Node() {}
	Node(string s, set<int> ids): str(s), belongto(ids) {}
	string str = "";
	set<int> belongto;
	vector<Node*> children;
};
class GST
{
public:
	GST() {r = new Node();}
	void Insert(string s, int id)
	{
		Node *p = r;
		for (auto n = p->children.begin(); n != p->children.end(); )
		{
			int i = 0;
			while (i < s.length() && i < (*n)->str.length() && s[i] == (*n)->str[i])
			{
				++i;
			}
			if (i == s.length() && s.length() == (*n)->str.length()) /*s == n->str*/
			{
				(*n)->belongto.insert(id);
				return;
			}
			else if (i == s.length())		/* s 包含于 n->str */
			{
				Node *tmp = new Node(s, {id});
				tmp->belongto.insert((*n)->belongto.begin(), (*n)->belongto.end());
				tmp->children.push_back(*n);
				(*n)->str = (*n)->str.substr(s.length());
				p->children.erase(n);
				p->children.push_back(tmp);
				return;
			}
			else if (i == (*n)->str.length())	/* s 包含 n->str */
			{
				s = s.substr((*n)->str.length());
				(*n)->belongto.insert(id);
				p = *n;
				n = p->children.begin();
			}
			else if (i == 0)				/* s 与 n->str 完全不同 */
			{
				++n;
			}
			else							/* s 与 n->str 有公共前缀 */
			{
				string cs = s.substr(0, i);
				Node *tmp = new Node(cs, {id});
				tmp->belongto.insert((*n)->belongto.begin(), (*n)->belongto.end());
				tmp->children.push_back(*n);
				(*n)->str = (*n)->str.substr(cs.length());
				Node *tmp2 = new Node(s.substr(cs.length()), {id});
				tmp->children.push_back(tmp2);
				p->children.erase(n);
				p->children.push_back(tmp);
				return;
			}
		}
		//如果p没有子节点 或者 完全不相同
		
		Node * tmp = new Node(s, {id});
		p->children.push_back(tmp);			
		
		return;
		
	}
	void display(Node * p)
	{
		cout << string(count++, '-');
		cout << p->str << ":{";
		for (const auto &i: p->belongto)
		{
			cout << i << " ";
		}
		cout << "}" << endl;
		for (const auto &n: p->children)
		{
			display(n);
		}
		--count;
	}
	void display()
	{
		display(r);
	}

	string search(int ids, Node *p)
	{
		string tmp;
		vector<string> strs;
		if (p->belongto.size() == ids || p->str == "")
		{
			for (const auto &n: p->children)
			{
				string res = search(ids, n);
				if (res != "") strs.push_back(res);
			}
			int mi = 0, mlen = 0;
			for (int i = 0; i < strs.size(); ++i)
			{
				if (strs[i].length() > mlen)
				{
					mlen = strs[i].length();
					mi = i;
				}
			}
			if (mlen != 0)
			{
				tmp = p->str + strs[mi];
			}
			else
			{
				tmp = p->str;
			} 
		}
		return tmp;
	}
	string Search(int idCounts)
	{
		return search(idCounts, r);
	}
private:
	Node *r = nullptr;
	int count = 0;
};

string LCS3(vector<string> strs)
{
	GST gst;
	for (int i = 0; i < strs.size(); ++i)
	{
		for (int j = strs[i].length() - 1; j >= 0; --j)
		{
			gst.Insert(strs[i].substr(j), i);
		}
	}
	cout << "insert finished" << endl;
	gst.display();
	cout << "display finished" << endl;
	return gst.Search(strs.size());
}

int main()
{
	string s1 = "21232523311324";
	string s2 = "312123223445";
	//cout << "lcs1: " << LCS1(s1, s2) << endl;
	//cout << "lcs2: " << LCS2(s1, s2) << endl;
	vector<string> strs = {
		"abcde",
		"cdef",
		"ccde"
	};
	
	cout << LCS3(strs) << endl;
	return 0;
}
