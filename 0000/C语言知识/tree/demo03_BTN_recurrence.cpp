#include <iostream>
#include "demo01_BTNodeClass.cpp"
using namespace std;
template <typename T>class Post
{
	/*******************************�����Ƕ������ķǵݹ��㷨************************************************/
public:
	void CreateBT2(BTNodeClass<T> &obj,T *prestr,T *instr,int n);//����������������ȷ��������,ʹ�õݹ��㷨
	void CreateBT3(BTNodeClass<T> &obj,T *posstr,T *instr,int n);//���ú�����������ȷ��������,ʹ�õݹ��㷨

	//****************************���㷨ȫ�����ö�ջ�����ݽṹ********************************************

	void NPreOrder1(BTNodeClass<T> &obj);//��������ĵ�һ���㷨
	void NPreOrder2(BTNodeClass<T> &obj);//��������ĵڶ����㷨
	void NInOrder1(BTNodeClass<T> &obj);//����������㷨
	void NPostOrder1(BTNodeClass<T> &obj);//���������ĵ�һ���㷨
	bool Ancestor31(BTNodeClass<T> &obj,T x);//���ú����������ж�ĳһ�ڵ�����Ƚڵ�����
	//***************************************************************************************

	//*************************���ö��е�˼�밴�ղ�α����������*****************************
	void LevelOrder1(BTNodeClass<T> &obj);//��α����㷨
	bool Ancestor32(BTNodeClass<T> &obj,T x);//���x�ڵ���������Ƚڵ㣬�ò�α�����˼��
	
private:
	BTNode<T> * CreateBT21(T *prestr,int ipre,T *instr,int iin,int n);//����������������ȷ����������ʹ�õݹ��㷨
	BTNode<T> * CreateBT31(T *posstr,int ipos,T *instr,int iin,int n);//���ú�����������ȷ����������ʹ�õݹ��㷨

	//****************************���㷨ȫ�����ö�ջ�����ݽṹ********************************************
	void NPreOrder11(BTNode<T> *t);
	void NPreOrder22(BTNode<T> *t);
	void NInOrder11(BTNode<T> *t);
	void NPostOrder11(BTNode<T> *t);
	bool Ancestor311(BTNode<T> *t,T x);
	/********************************�ǵݹ��㷨�ĳ�Ա����***************************************************/

	//*************************���ö��е�˼�밴�ղ�α����������*****************************
	void LevelOrder11(BTNode<T> *t);
	bool Ancestor322(BTNode<T> *t,T x);

};
template <typename T>struct QUType
{
	BTNode<T> *node;
	int parent;
};
//������������������������������
template <typename T>void Post<T>::CreateBT3(BTNodeClass<T> &obj,T *poststr,T *instr,int n)
{
	obj.r = CreateBT31(poststr,0,instr,0,n);
}
template <typename T>BTNode<T> * Post<T>::CreateBT31(T *poststr,int ipost,T *instr,int iin,int n)
{
	BTNode<T> *t;
	char ch;
	int p,k;
	if(n <= 0) return NULL;
	ch = poststr[ipost+n-1];
	t = new BTNode<T>;
	t->data = ch;
	p = iin;
	while(p <iin+n)
	{
		if(instr[p] == ch)
		{
			break;
		}
		p++;
	}
	k = p - iin;
	t->lchild = CreateBT21(poststr,ipost,instr,iin,k);
	t->rchild = CreateBT21(poststr,ipost+k,instr,p+1,n-k-1);
	return t;
}
//������������������������������
template <typename T>void Post<T>::CreateBT2(BTNodeClass<T> &  ,T *prestr,T *instr,int n)
{
	obj.r = CreateBT21(prestr,0,instr,0,n);
}
template <typename T>BTNode<T> * Post<T>::CreateBT21(T *prestr,int ipre,T *instr,int iin,int n)
{
	BTNode<T> *t;
	char ch;
	int p,k;
	if(n <= 0) return NULL;
	ch = prestr[ipre];
	t = new BTNode<T>;
	t->data = ch;
	p = iin;
	while(p <iin+n)
	{
		if(instr[p] == ch)
		{
			break;
		}
		p++;
	}
	k = p - iin;
	t->lchild = CreateBT21(prestr,ipre+1,instr,iin,k);
	t->rchild = CreateBT21(prestr,ipre+k+1,instr,p+1,n-k-1);
	return t;
}
//������е����Ƚڵ㡣���ö��У�
template <typename T>bool Post<T>::Ancestor32(BTNodeClass<T> &obj,T x)
{
	return Ancestor322(obj.r,x);
}
template <typename T>bool Post<T>::Ancestor322(BTNode<T> *t,T x)
{
	BTNode<T> *p;
	QUType<T> qu[MAXSIZE];//�����ѭ������
	int front,rear,i;
	front = rear = -1;//����Ϊ-1ʱ��ʾ����Ϊ��
	rear++;qu[rear].node = t;//��ͷ�ڵ������
	qu[rear].parent = -1;//��˫�׽ڵ���Ϊһ
	while(front != rear)//��������л���Ԫ�أ������ѭ��
	{
		//rear��¼����һ��Ԫ�ص��±�
		front++;p = qu[front].node;//������
		//front��������¼����һ��Ԫ�ؽ�����ʱ���±꣬���ҵ���Ԫ�ؽ�����ʱ���������ڵ���±�����ӽڵ�
		//���ӽڵ��¼���ڵ���±꣬��������Ƚڵ�ʱ������ֻ��ҪѰ�ҵ�ǰ�ڵ��parent��������ѵ�ǰ�ڵ��parent
		//����ԭ�ȶ��е��±꣬�Ϳ����ҵ����ĸ��ڵ㣬�Դ�����

		if(p->data == x)//�ж��Ƿ����
		{
			i = qu[front].parent;
			if(i != -1)
			{
				while(qu[i].parent != -1)//��ӡ�������Ƚڵ㣬���˸��ڵ�
				{
					cout<<qu[i].node->data<<" ";
					i = qu[i].parent;
				}
				cout<<qu[i].node->data;//��ӡ���ڵ㣬��Ϊ�������i=0ʱ�����ڵ��parent�Ѿ������-1���������������˳���ѭ��
				//�����ڵ�Ҳ����������������Ƚڵ㣬���Դ�ӡ��
				return true;
			}
		}
		//����Ԫ��ʱ������������������
		if(p->lchild != NULL)
		{
			rear++;qu[rear].node = p->lchild;
			qu[rear].parent = front;//�����׽ڵ���±긳����������parent��
		}
		if(p->rchild != NULL)
		{
			rear++;qu[rear].node = p->rchild;
			qu[rear].parent = front;
		}
	}
	return false;
}
//���ö��е�˼����в�α���
template <typename T>void Post<T>::LevelOrder1(BTNodeClass<T> &obj)
{
	LevelOrder11(obj.r);
}
template <typename T>void Post<T>::LevelOrder11(BTNode<T> *t)
{
	BTNode<T> *qu[MAXSIZE];
	int rear = 0,front = 0;//����������ʱ�±�
	rear = (rear+1)%MAXSIZE;//�������ݼ�һ
	qu[rear] = t;//��ͷ�ڵ����
	while(rear != front)
	{
		front = (front+1)%MAXSIZE;//ʲô�Ƕ��У����о����Ƚ��ȳ��������������Ҳ����ģ��һ������
		//����ֻ��Ҫ����һ�����ֵȡ��Ϳ���
		cout<<qu[front]->data<<" ";//������Ƚ�������
		//����У��ͽ����У����±����
		if(qu[front]->lchild != NULL)
		{
			rear = (rear + 1)%MAXSIZE;
			qu[rear] = qu[front]->lchild;
		}
		//����У��ͽ����У����±����
		if(qu[front]->rchild != NULL)
		{
			rear = (rear + 1)%MAXSIZE;
			qu[rear] = qu[front]->rchild;
		}
	}
}
//���ú����������㷨��ʵ�ֶ����Ƚڵ�����
template <typename T>bool Post<T>::Ancestor31(BTNodeClass<T> &obj,T x)
{
	return Ancestor311(obj.r,x);
}
template <typename T>bool Post<T>::Ancestor311(BTNode<T> *t,T x)
{
	BTNode <T> * buf[MAXSIZE];
	BTNode<T> *p = t,*q;
	int top = -1;
	int i = 0;
	bool flag;
	do 
	{
		while(p != NULL)
		{
			top++;buf[top] = p;
			p = p->lchild;
		}
		flag = true;
		q = NULL;
		while (top != -1 && flag == true)//ֻ����������ͬʱ�������������½���
		{
 			if(p->rchild == q)
			{
				if(p->data == x)//�ж������Ҫ�ҵĽڵ㣬����������Ƚڵ�
				{
					for (i = 0; i< top; i++)
					{
						cout<<buf[i]->data<<" ";
					}
					return true;//�ҵ�����true��
				}
				else
				{
					top--;//���û���ҵ�������ջ
					q = p;
				}
			}
			else 
			{
				p = p->rchild;
				flag = false;
			}
		}
	} while (top != -1);
	return false;//�Ҳ�������false
}
//���������ķǵݹ��㷨
template <typename T>void Post<T>::NPostOrder1(BTNodeClass<T> &obj)
{
	NPostOrder11(obj.r);
}
template <typename T>void Post<T>::NPostOrder11(BTNode<T> *t)
{//{
	BTNode<T> *buf[MAXSIZE];
	BTNode<T>* p = NULL;
	int top = -1;
	p = t;
	//���������㷨
	bool flag;//������־λ�������ж��������Ƿ�Ϊ��
	BTNode<T> *q;
	do 
	{
		while(p != NULL)//�ж��������Ƿ�Ϊ��
		{
			top++;buf[top] = p;//��������ȫ����ջ
			p = p->lchild;
		}
		q = NULL;//��������ж��������Ƿ�ΪNULL
		flag = true;//������־λ����־�������Ѿ�Ϊ��
		while(top != -1 && flag == true)//�ж�ջ�Ƿ�Ϊ�գ������������Ѿ�Ϊ��
		{
			p = buf[top];//��ջ�е�Ԫ��ȡ����������p��top--����д������
			//��Ϊ�������ǰ�ڵ������������һ����ջ�Ľڵ���ͬʱ
			//��û����Ҫ������ջ��

			if(p->rchild == q)//�ж��������Ƿ�Ϊ��
			{
				cout<<p->data<<" ";//��ӡ����û�����������������Ľڵ�
				top--;
				q = p;//����һ�δ�ӡ�Ľڵ��¼����
			}else
			{
				p = p->rchild;//��ָ������
				flag = false;//����־λ����Ϊfalse����˼����������Ϊ��
				//���ҽ�ѭ���Ƴ����ظ�do whileѭ��,���������ж����������������Ƿ�Ϊ��
			}
		}
	} while (top != -1);//�ж�ջ�Ƿ�Ϊ��

}
//��������ķǵݹ��㷨
template <typename T>void Post<T>::NInOrder1(BTNodeClass<T> &obj)
{
	NInOrder11(obj.r);
}
template <typename T>void Post<T>::NInOrder11(BTNode<T> *t)
{
	BTNode<T> *buf[MAXSIZE];
	BTNode<T> *p = NULL;
	p = t;
	int top = -1;
	while(top != -1 || p != NULL)//�ж�ջ�Ƿ�Ϊ��
	{
		while(p != NULL)//�ж��������Ƿ�Ϊ��
		{
			top++;buf[top] = p;//�����Ϊ�գ����ջ
			p = p->lchild;
		}
		if(top != -1)//�ж�ջ���Ƿ���Ԫ��
		{
			p = buf[top];top--;//�õ�û��������Ϊ�յĸ��׽ڵ�
			cout<<p->data<<" ";//�����ǰ�ڵ�
			p = p->rchild;//�ҵ�ǰ�ڵ����������������������p���ٶ��������ж�
		}
	}
}
//ǰ��ǵݹ��㷨
template <typename T>void Post<T>::NPreOrder2(BTNodeClass<T> &obj)
{
	NPreOrder22(obj.r);
}
template <typename T>void Post<T>::NPreOrder22(BTNode<T> *t)
{
	BTNode<T> *buf[MAXSIZE];
	int top = -1;
	BTNode<T>* p = NULL;
	p = t;
	while(top != -1 || p != NULL)//��������Ȼtop != -1 �������������߳��������˳������������
	{
		while(p != NULL)//�ж��������Ƿ�Ϊ��
		{
			cout<<p->data<<" ";//������ڵ�
			top++;buf[top] = p;//�����ڵ��ջ
			p = p->lchild;
			//Ѱ����һ��������======>����Ѿ����������������֮��p= NULL;����������뽫buf�е�Ԫ�ؼ����г�ջ
		}
		if(top != -1)
		{
			p = buf[top];top--;//�������г�ջ����
			p = p->rchild;//�ж�������
			//����һ�仰ִ�н�������������ȥ�����жϣ���ǰp�ͳ��˸��ڵ�
		}
	}
}
template <typename T>void Post<T>::NPreOrder1(BTNodeClass<T> &obj)
{
	NPreOrder11(obj.r);
}
template <typename T>void  Post<T>::NPreOrder11(BTNode<T> *t)
{
	BTNode<T> *buf[MAXSIZE];//����һ��˳��ջ
	int top = -1;
	BTNode<T> *p = NULL;//����һ��ָ�����ͣ������������нڵ�
	top++;
	buf[top] = t;//��ͷ�ڵ������str[top]������Ŀ����ѭ����
	//����һ���Ƕȿ���Ҳ����ͷ�ڵ��ջ
	while(top != -1)
	{
		p = buf[top];top--;//pʼ���ǳ�ջ��Ԫ��
		cout<<p->data<<" ";
		if(p->rchild != NULL)//�����������Ϊ��������������ջ
		{
			top++;
			buf[top] = p->rchild;//��ջ����
		}
		if(p->lchild != NULL)//�����������Ϊ��������������ջ
		{
			top++;
			buf[top] = p->lchild;//��ջ����
		}
	}
}
void main()
{
	BTNodeClass<char> mych; 
	BTNodeClass<char> pch;
	Post<char> q2;
	char *prestr = "ABDGCEF";
	char *instr = "DGBAECF";
	char *poststr = "GDBEFCA";
	int len = strlen(prestr);
	char *mystr = "A(B(D(g,f(,G)),K(L,S)),C(E,F))";
	cout<<len<<endl;
	BTNode<char> *temp = NULL;
	mych.CreatBTNode(mystr);
	cout<<"ǰ���������"<<endl;
	q2.NPreOrder1(mych);
	cout<<"\nǰ���������"<<endl;
	q2.NPreOrder2(mych);
	cout<<"\n�������������£�"<<endl;
	q2.NInOrder1(mych);
	cout<<"\n��������������£�"<<endl;
	q2.NPostOrder1(mych);
	cout<<endl;
	cout<<"���е����Ƚڵ��ǣ�"<<endl;
	q2.Ancestor31(mych,'g');
	cout<<"\n���ö��е�˼����в�α���"<<endl;
	q2.LevelOrder1(mych);
	cout<<"\n���ò�α����ķ����������е����Ƚڵ�"<<endl;
	q2.Ancestor32(mych,'G');
	cout<<endl;
	q2.CreateBT2(pch,prestr,instr,len);
	cout<<"\nǰ���������"<<endl;
	q2.NPreOrder2(pch);
	cout<<"\n�������������£�"<<endl;
	q2.NInOrder1(pch);
	q2.CreateBT3(pch,poststr,instr,len);
	cout<<"\nǰ���������"<<endl;
	q2.NPreOrder2(pch);
	cout<<"\n�������������£�"<<endl;
	q2.NInOrder1(pch);
	system("pause");
}
