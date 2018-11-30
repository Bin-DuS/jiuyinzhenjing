//#include <iostream>
//#include <stack>
//using namespace std;

#define MAXSIZE 20

template <typename T>
struct BTNode
{
	T data;
	BTNode *lchild,*rchild;
};
/********************************һ���Ƕ������ĵݹ��㷨��Ա����*******************************************/
template <typename T>
class BTNodeClass
{
public:
	BTNode<T> *r;
public:
	BTNodeClass();
	~BTNodeClass();
	void CreatBTNode(char *str);
	BTNode<T> *FindNode(T x);
	void DispBTNode();
	int BTNodeHeight();
	void DispInOeder();
	//�����ڵ�
	void PreOrder(BTNodeClass<T> &obj);
	void InOrder(BTNodeClass<T> &obj);
	void PostOrder(BTNodeClass<T> &obj);

private:
	void DestroyBTree(BTNode<T> *r);
	void DispBTNode1(BTNode<T> *t);
	void DispInOeder1(BTNode<T> *t);
	BTNode<T> *FindNode1(BTNode<T> *t,T x);
	int BTNodeHeight1(BTNode<T> *t);
	//�����ڵ�
	void PreOrder1(BTNode<T> *t);
	void InOrder1(BTNode<T> *t);
	void PostOrder1(BTNode<T> *t);

protected:
};
/********************************************�����Ƕ������ĵݹ��㷨��*************************************/
