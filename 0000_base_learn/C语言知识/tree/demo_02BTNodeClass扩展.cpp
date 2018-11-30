#include <iostream>
#include "demo01_BTNodeClass.cpp"
using namespace std;

//��һ���㷨����������нڵ����
template <typename T>
class A
{
	//********************************************�ݹ��㷨******************************************/
public:
	int NodeCount1(BTNodeClass<T> &obj);
	int NodeCount2(BTNodeClass<T> &obj);
	void DispLeaf(BTNodeClass<T> &obj);
	void CopyTree(BTNodeClass<T> &obj_0,BTNodeClass<T> &obj_1);
	//void Trans(char sqbstree[],BTNodeClass<T> &obj);

	int level(BTNodeClass<T> &obj,T x);
	bool Ancestor(BTNodeClass<T> &obj,T x);//Ѱ��x�ڵ�����нڵ�


private:
	int NodeCount11(BTNode<T> *t);
	int NodeCount21(BTNode<T> *t);
	void DispLeaf1(BTNode<T> *t);
	void CopyTree1(BTNode<T> *t_0,BTNode<T> *t_1);
	//void Trans1(char sqbstree[],int i,BTNode<T> *&t);//����һ��ָ�룬һ��BTNode���͵�ָ�룬&tֻ����ʱһ��������
	//������˼�ǣ����������ı�����������һ�����֣�û��������������壬
	int level1(BTNode<T> *t,T x,int H);
	bool Ancestor1(BTNode<T> *t,T x);//Ѱ��x�ڵ�����нڵ�
	/******************************************����Ϊ�ݹ���*************************************************/

};

//*****************************************һ��Ϊ�������ĵݹ��㷨******************************************/
template <typename T>
//�������
int A<T>::NodeCount1(BTNodeClass<T> &obj)
{
	return NodeCount11(obj.r);
}
template <typename T>int A<T>::NodeCount11(BTNode<T> *t)
{
	int m,n,k;
	if(t != NULL)
	{
		k = 1;//�������
		m = NodeCount11(t->lchild);
		//k = 1;�������
		n = NodeCount11(t->rchild);
		//k = 1;�������
		return k+m+n;
	}
	else return 0;
}


//��ȫ���õݹ�����ԣ������������Ҷ������
template <typename T>int  A<T>::NodeCount2(BTNodeClass<T> &obj)
{
	return NodeCount21(obj.r);
}
template <typename T>int  A<T>::NodeCount21(BTNode<T> *t)
{
	if(t == NULL) return 0;
	else 
		return (NodeCount21(t->lchild)+NodeCount21(t->rchild)+1);
}


//�����Ҷ��ͨ���ݹ�ķ���
template <typename T>void  A<T>::DispLeaf(BTNodeClass<T> &obj)
{
	return DispLeaf1(obj.r);
}
template <typename T>void  A<T>::DispLeaf1(BTNode<T> *t)
{
	if(t != NULL)
	{
		if(t->lchild == NULL && t->rchild == NULL)//�������������������Ϊ�գ���˵������һ����Ҷ
			cout<<t->data<<" ";
		DispLeaf1(t->lchild);//�ݹ�����������Ѱ����Ҷ
		DispLeaf1(t->rchild);
	}
}

template <typename T>void  A<T>::CopyTree(BTNodeClass<T> &obj_0,BTNodeClass<T> &obj_1)
{
	CopyTree1(obj_0.r,obj_1.r);
}
template <typename T>void  A<T>::CopyTree1(BTNode<T> *t_0,BTNode<T> *t_1)
{
	if(t_0 != NULL)
	{
		t_1 = new BTNode<T>();
		t_1->data = t_0->data;
		CopyTree1(t_0->lchild,t_1->lchild);
		CopyTree1(t_0->rchild,t_1->rchild);
	}
	else t_1 = NULL;
}
template <typename T>int A<T>::level(BTNodeClass<T> &obj,T x)//���x���ڶ������Ĳ�Σ�H��
{
	return level1(obj.r,x,1);//1��һ����ʼֵ
}
template <typename T>int A<T>::level1(BTNode<T> *t,T x,int H)//H��ÿ�ε���ʱ�������˱仯������һ���ֵ
{
	int l;
	if(t == NULL) return 0;
	else if(t->data == x) return H;
	else 
	{
		l = level1(t->lchild,x,H+1);//����������������ڣ��򷵻�����
		if(l != 0) return l;
		else return (level1(t->rchild,x,H+1));//����������
	}
}
template <typename T>bool A<T>::Ancestor(BTNodeClass<T> &obj,T x)//Ѱ��x�ڵ�����нڵ�
{
	return Ancestor1(obj.r,x);
}
template <typename T>bool A<T>::Ancestor1(BTNode<T> *t,T x)//Ѱ��x�ڵ�����нڵ�      
{	//�����ǰ�ڵ�Ϊ�գ���ʧ��
	if(t == NULL) return false;
	//���x�����ӽڵ��ֵ�պ���ȣ��������ǰ�ڵ�
	if(t->lchild != NULL && t->lchild->data == x)
	{
		cout<<t->data<<" ";
		return true;
	}
	//���x�����ӽڵ��ֵ�պ���ȣ��������ǰ�ڵ�
	if(t->rchild != NULL && t->rchild->data == x)
	{
		cout<<t->data<<" ";
		return true;
	}
	//���û���ڵڶ����ҵ���������ݹ�Ѱ��
	//���ڵݹ�����Կ��Եõ��������ʱ�պ���һ�������
	//Ҳ����˵�����ݹ���ҵ�Ԫ��ʱ���ݹ���˳������պ�������˳�������ʾ�����
	if(Ancestor1(t->lchild,x) || Ancestor1(t->rchild,x))
	{
		cout<<t->data<<" ";
		return true;
	}else
		return false;
}
//void main21()
//{
//	btnodeclass<char> mych,copymych; 
//	char *mystr = "a(b(d(g,f(,g)),),c(e,f))";
//	char sqbstree[] = "abc###d##";
//
//	btnode<char> *temp = null;
//	mych.creatbtnode(mystr);
//	cout<<"ǰ���������"<<endl;
//	mych.dispbtnode();
//	mych.inorder(mych);
//	cout<<endl;
//	a<char> q1;
//	cout<<q1.nodecount1(mych);//�������
//	cout<<endl;
//	cout<<q1.nodecount2(mych);//���õݹ����ԣ���������ҹ����
//	cout<<endl;
//	q1.displeaf(mych);//�����Ҷ��ͨ���ݹ�÷���
//	cout<<endl;
//	q1.copytree(mych,copymych);
//	cout<<"copy �������£�"<<endl;
//	mych.dispbtnode();
//	mych.inorder(mych);
//	cout<<endl;
//
//	//q1.trans(sqbstree,mych);
//	cout<<q1.level(mych,'g');
//	cout<<endl;
//	q1.ancestor(mych,'g');
//	system("pause");
//}