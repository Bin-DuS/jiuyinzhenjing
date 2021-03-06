/**
 * C语言实现的红黑树(Red Black Tree)
 *
 * @author skywang
 * @date 2013/11/18
 */

#include <stdio.h>
#include "inc/rbtree.h"

#define CHECK_INSERT 0    // "插入"动作的检测开关(0，关闭；1，打开)
#define CHECK_DELETE 0    // "删除"动作的检测开关(0，关闭；1，打开)
#define LENGTH(a) ( (sizeof(a)) / (sizeof(a[0])) )
void test_2(){
	RBRoot * root=NULL;
	root = create_rbtree();
	int i = 0;
	for(i = 0;i<1000;i++){
		insert_rbtree(root,i);
	}
	print_rbtree(root);
	printf("after delete 500\n");
	for(i = 0;i<500;i++){
		delete_rbtree(root, i);
	}
	print_rbtree(root);
	printf("after delete 750\n");
	for(i = 500;i<750;i++){
		delete_rbtree(root, i);
	}
	print_rbtree(root);
	destroy_rbtree(root);
}
void test_1(){
	int a[] = {10, 40, 30, 60, 90, 70, 20, 50, 80};
    int i, ilen=LENGTH(a);
    RBRoot *root=NULL;

    root = create_rbtree();
    printf("== 原始数据: ");
    for(i=0; i<ilen; i++)
        printf("%d ", a[i]);
    printf("\n");

    for(i=0; i<ilen; i++)
    {
        insert_rbtree(root, a[i]);
//#if CHECK_INSERT
        printf("== 添加节点: %d\n", a[i]);
        printf("== 树的详细信息: \n");
        print_rbtree(root);
        printf("\n");
//#endif
    }

    printf("== 前序遍历: ");
    preorder_rbtree(root);

    printf("\n== 中序遍历: ");
    inorder_rbtree(root);

    printf("\n== 后序遍历: ");
    postorder_rbtree(root);
    printf("\n");

    if (rbtree_minimum(root, &i)==0)
        printf("== 最小值: %d\n", i);
    if (rbtree_maximum(root, &i)==0)
        printf("== 最大值: %d\n", i);
    printf("== 树的详细信息: \n");
    print_rbtree(root);
    printf("\n");

//#if CHECK_DELETE
    for(i=0; i<ilen; i++)
    {
        delete_rbtree(root, a[i]);

        printf("== 删除节点: %d\n", a[i]);
        if (root)
        {
            printf("== 树的详细信息: \n");
            print_rbtree(root);
            printf("\n");
        }
    }
//#endif

    destroy_rbtree(root);
}
void main()
{
	test_1();
	//test_2();
}
/*
参考资料 
http://www.cnblogs.com/skywang12345/p/3245399.html
https://www.cnblogs.com/skywang12345/p/3624177.html
	红黑树的主要操作，左旋 右旋 插入 删除
	特性：
		1.每个节点或者是黑色或者是红色
		2.根节点是黑色
		3.每个叶子节点(NIL)是黑色，
			【注意】：叶子节点指为空（NIL或者NULL）的叶子节点，如每个节点的左右孩子指针为空的情况下 这个孩子指针即为叶子节点
		4.如果一个节点是红色的，他的子节点必须是黑色
		5.从一个节点到该节点的子孙节点的所有路径上包含相同数目的黑节点
		参见rbtree.jpg
	添加或者删除节点会破坏红黑树的属性，需要通过旋转(左旋，右旋)使树保持红黑树的特性
	树的左支上的值都比当前节点小，右支都比当前节点大
	红黑树旋转主要为了应对树的变异，一支特别长，一支特别短，退化为链表。查询时间会变大，通过及时旋转保证树的平衡
	左旋:（右旋是左旋的逆反）
		旋转前a<x<b<y<c:     旋转后 a<x<b<y<c
		|                     |
		x       左旋          y
	   / \     ----->        / \
	  a   y     右旋        x   c
	     / \   <-----      / \
		b   c             a   b
	对x进行左旋，意味着，将“x的右孩子”设为“x的父亲节点”；即，将 x变成了一个左节点(x成了为z的左孩子)！。 因此，左旋中的“左”，意味着“被旋转的节点将变成一个左节点”。	
	对x进行右旋，意味着，将“x的左孩子”设为“x的父亲节点”；即，将 x变成了一个右节点(x成了为y的右孩子)！ 因此，右旋中的“右”，意味着“被旋转的节点将变成一个右节点”。
	红黑树添加的步骤
		1.将红黑树当做二叉查找树，将节点插入
			红黑树本身就是一颗二叉查找树，将节点插入后，该树仍然是一颗二叉查找树。也就意味着，树的键值仍然是有序的。此外，无论是左旋还是右旋，若旋转之前这棵树是二叉查找树，旋转之后它一定还是二叉查找树。这也就意味着，任何的旋转和重新着色操作，都不会改变它仍然是一颗二叉查找树的事实。
		2.将节点着色为红色
			将插入的节点着色为红色，不会违背"特性(5)"！少违背一条特性，就意味着我们需要处理的情况越少。接下来，就要努力的让这棵树满足其它性质即可；满足了的话，它就又是一颗红黑树了。
		3.通过旋转和重新着色来修正该树，使之成为红黑树。
			如树进行初始化，或者大量添加，删除（破坏树结构），是否可以在破坏完毕后再进行旋转，着色。
			第二步中，将插入节点着色为"红色"之后，不会违背"特性(5)"。那它到底会违背哪些特性呢？
			对于"特性(1)"，显然不会违背了。因为我们已经将它涂成红色了。
			对于"特性(2)"，显然也不会违背。在第一步中，我们是将红黑树当作二叉查找树，然后执行的插入操作。而根据二叉查找数的特点，插入操作不会改变根节点。所以，根节点仍然是黑色。
			对于"特性(3)"，显然不会违背了。这里的叶子节点是指的空叶子节点，插入非空节点并不会对它们造成影响。
			对于"特性(4)"，是有可能违背的！
		那接下来，想办法使之"满足特性(4)"，就可以将树重新构造成红黑树了。
		根据被插入节点的父节点的情况，可以将"当节点z被着色为红色节点，并插入二叉树"划分为三种情况来处理。
			① 情况说明：被插入的节点是根节点。
				处理方法：直接把此节点涂为黑色。
			② 情况说明：被插入的节点的父节点是黑色。
				处理方法：什么也不需要做。节点被插入后，仍然是红黑树。
			③ 情况说明：被插入的节点的父节点是红色。
				处理方法：那么，该情况与红黑树的“特性(5)”相冲突。这种情况下，被插入节点是一定存在非空祖父节点的；进一步的讲，被插入节点也一定存在叔叔节点(即使叔叔节点为空，我们也视之为存在，空节点本身就是黑色节点)。理解这点之后，我们依据"叔叔节点的情况"，将这种情况进一步划分为3种情况(Case)。
				Case 1 	当前节点的父节点是红色，且当前节点的祖父节点的另一个子节点（叔叔节点）也是红色。 	
					(01) 将“父节点”设为黑色。
					(02) 将“叔叔节点”设为黑色。
					(03) 将“祖父节点”设为“红色”。
					(04) 将“祖父节点”设为“当前节点”(红色节点)；即，之后继续对“当前节点”进行操作。
				Case 2 	当前节点的父节点是红色，叔叔节点是黑色，且当前节点是其父节点的右孩子 	
					(01) 将“父节点”作为“新的当前节点”。
					(02) 以“新的当前节点”为支点进行左旋。
				Case 3 	当前节点的父节点是红色，叔叔节点是黑色，且当前节点是其父节点的左孩子 	
					(01) 将“父节点”设为“黑色”。
					(02) 将“祖父节点”设为“红色”。
					(03) 以“祖父节点”为支点进行右旋。
		1. (Case 1)叔叔是红色
		1.1 现象说明
			当前节点(即，被插入节点)的父节点是红色，且当前节点的祖父节点的另一个子节点（叔叔节点）也是红色。
		1.2 处理策略
			(01) 将“父节点”设为黑色。
			(02) 将“叔叔节点”设为黑色。
			(03) 将“祖父节点”设为“红色”。
			(04) 将“祖父节点”设为“当前节点”(红色节点)；即，之后继续对“当前节点”进行操作。
			下面谈谈为什么要这样处理。(建议理解的时候，通过rbtree_insert_case1.jpg图进行对比)
			“当前节点”和“父节点”都是红色，违背“特性(4)”。所以，将“父节点”设置“黑色”以解决这个问题。
			但是，将“父节点”由“红色”变成“黑色”之后，违背了“特性(5)”：因为，包含“父节点”的分支的黑色节点的总数增加了1。  解决这个问题的办法是：将“祖父节点”由“黑色”变成红色，同时，将“叔叔节点”由“红色”变成“黑色”。关于这里，说明几点：第一，为什么“祖父节点”之前是黑色？这个应该很容易想明白，因为在变换操作之前，该树是红黑树，“父节点”是红色，那么“祖父节点”一定是黑色。 第二，为什么将“祖父节点”由“黑色”变成红色，同时，将“叔叔节点”由“红色”变成“黑色”；能解决“包含‘父节点’的分支的黑色节点的总数增加了1”的问题。这个道理也很简单。“包含‘父节点’的分支的黑色节点的总数增加了1” 同时也意味着 “包含‘祖父节点’的分支的黑色节点的总数增加了1”，既然这样，我们通过将“祖父节点”由“黑色”变成“红色”以解决“包含‘祖父节点’的分支的黑色节点的总数增加了1”的问题； 但是，这样处理之后又会引起另一个问题“包含‘叔叔’节点的分支的黑色节点的总数减少了1”，现在我们已知“叔叔节点”是“红色”，将“叔叔节点”设为“黑色”就能解决这个问题。 所以，将“祖父节点”由“黑色”变成红色，同时，将“叔叔节点”由“红色”变成“黑色”；就解决了该问题。
			按照上面的步骤处理之后：当前节点、父节点、叔叔节点之间都不会违背红黑树特性，但祖父节点却不一定。若此时，祖父节点是根节点，直接将祖父节点设为“黑色”，那就完全解决这个问题了；若祖父节点不是根节点，那我们需要将“祖父节点”设为“新的当前节点”，接着对“新的当前节点”进行分析。
		2. (Case 2)叔叔是黑色，且当前节点是右孩子
			2.1 现象说明
				当前节点(即，被插入节点)的父节点是红色，叔叔节点是黑色，且当前节点是其父节点的右孩
			2.2 处理策略
				(01) 将“父节点”作为“新的当前节点”。
				(02) 以“新的当前节点”为支点进行左旋。
				下面谈谈为什么要这样处理。(建议理解的时候，通过rbtree_insert_case2.jpg图进行对比)
				首先，将“父节点”作为“新的当前节点”；接着，以“新的当前节点”为支点进行左旋。 为了便于理解，我们先说明第(02)步，再说明第(01)步；为了便于说明，我们设置“父节点”的代号为F(Father)，“当前节点”的代号为S(Son)。
				为什么要“以F为支点进行左旋”呢？根据已知条件可知：S是F的右孩子。而之前我们说过，我们处理红黑树的核心思想：将红色的节点移到根节点；然后，将根节点设为黑色。既然是“将红色的节点移到根节点”，那就是说要不断的将破坏红黑树特性的红色节点上移(即向根方向移动)。 而S又是一个右孩子，因此，我们可以通过“左旋”来将S上移！
				按照上面的步骤(以F为支点进行左旋)处理之后：若S变成了根节点，那么直接将其设为“黑色”，就完全解决问题了；若S不是根节点，那我们需要执行步骤(01)，即“将F设为‘新的当前节点’”。那为什么不继续以S为新的当前节点继续处理，而需要以F为新的当前节点来进行处理呢？这是因为“左旋”之后，F变成了S的“子节点”，即S变成了F的父节点；而我们处理问题的时候，需要从下至上(由叶到根)方向进行处理；也就是说，必须先解决“孩子”的问题，再解决“父亲”的问题；所以，我们执行步骤(01)：将“父节点”作为“新的当前节点”。
		3. (Case 3)叔叔是黑色，且当前节点是左孩子
			3.1 现象说明
			当前节点(即，被插入节点)的父节点是红色，叔叔节点是黑色，且当前节点是其父节点的左孩子
			3.2 处理策略
				(01) 将“父节点”设为“黑色”。
				(02) 将“祖父节点”设为“红色”。
				(03) 以“祖父节点”为支点进行右旋。
				下面谈谈为什么要这样处理。(建议理解的时候，通过rbtree_insert_case3.jpg图进行对比)
				为了便于说明，我们设置“当前节点”为S(Original Son)，“兄弟节点”为B(Brother)，“叔叔节点”为U(Uncle)，“父节点”为F(Father)，祖父节点为G(Grand-Father)。
				S和F都是红色，违背了红黑树的“特性(4)”，我们可以将F由“红色”变为“黑色”，就解决了“违背‘特性(4)’”的问题；但却引起了其它问题：违背特性(5)，因为将F由红色改为黑色之后，所有经过F的分支的黑色节点的个数增加了1。那我们如何解决“所有经过F的分支的黑色节点的个数增加了1”的问题呢？ 我们可以通过“将G由黑色变成红色”，同时“以G为支点进行右旋”来解决。
	红黑树的删除
		1.将红黑树当作一颗二叉查找树，将该节点从二叉查找树中删除；
			这和"删除常规二叉查找树中删除节点的方法是一样的"。分3种情况：
			① 被删除节点没有儿子，即为叶节点。那么，直接将该节点删除就OK了。
			② 被删除节点只有一个儿子。那么，直接删除该节点，并用该节点的唯一子节点顶替它的位置。
			③ 被删除节点有两个儿子。那么，先找出它的后继节点；然后把“它的后继节点的内容”复制给“该节点的内容”；之后，删除“它的后继节点”。在这里，后继节点相当于替身，在将后继节点的内容复制给"被删除节点"之后，再将后继节点删除。这样就巧妙的将问题转换为"删除后继节点"的情况了，下面就考虑后继节点。 在"被删除节点"有两个非空子节点的情况下，它的后继节点不可能是双子非空。既然"的后继节点"不可能双子都非空，就意味着"该节点的后继节点"要么没有儿子，要么只有一个儿子。若没有儿子，则按"情况① "进行处理；若只有一个儿子，则按"情况② "进行处理。
		2.通过"旋转和重新着色"等一系列来修正该树，使之重新成为一棵红黑树。
			因为"第一步"中删除节点之后，可能会违背红黑树的特性。所以需要通过"旋转和重新着色"来修正该树，使之重新成为一棵红黑树。
		前面我们将"删除红黑树中的节点"大致分为两步，在第一步中"将红黑树当作一颗二叉查找树，将节点删除"后，可能违反"特性(2)、(4)、(5)"三个特性。第二步需要解决上面的三个问题，进而保持红黑树的全部特性。
		为了便于分析，我们假设"x包含一个额外的黑色"(x原本的颜色还存在)，这样就不会违反"特性(5)"。为什么呢？
		通过RB-DELETE算法，我们知道：删除节点y之后，x占据了原来节点y的位置。 既然删除y(y是黑色)，意味着减少一个黑色节点；那么，再在该位置上增加一个黑色即可。这样，当我们假设"x包含一个额外的黑色"，就正好弥补了"删除y所丢失的黑色节点"，也就不会违反"特性(5)"。 因此，假设"x包含一个额外的黑色"(x原本的颜色还存在)，这样就不会违反"特性(5)"。
		现在，x不仅包含它原本的颜色属性，x还包含一个额外的黑色。即x的颜色属性是"红+黑"或"黑+黑"，它违反了"特性(1)"。

		现在，我们面临的问题，由解决"违反了特性(2)、(4)、(5)三个特性"转换成了"解决违反特性(1)、(2)、(4)三个特性"。RB-DELETE-FIXUP需要做的就是通过算法恢复红黑树的特性(1)、(2)、(4)。RB-DELETE-FIXUP的思想是：将x所包含的额外的黑色不断沿树上移(向根方向移动)，直到出现下面的姿态：
			a) x指向一个"红+黑"节点。此时，将x设为一个"黑"节点即可。
			b) x指向根。此时，将x设为一个"黑"节点即可。
			c) 非前面两种姿态。
			将上面的姿态，可以概括为3种情况。
				① 情况说明：x是“红+黑”节点。
					处理方法：直接把x设为黑色，结束。此时红黑树性质全部恢复。
				② 情况说明：x是“黑+黑”节点，且x是根。
					处理方法：什么都不做，结束。此时红黑树性质全部恢复。
				③ 情况说明：x是“黑+黑”节点，且x不是根。
					处理方法：这种情况又可以划分为4种子情况。这4种子情况如下表所示：
					现象说明 	处理策略
		Case 1 	x是"黑+黑"节点，x的兄弟节点是红色。(此时x的父节点和x的兄弟节点的子节点都是黑节点)。 	
			(01) 将x的兄弟节点设为“黑色”。
			(02) 将x的父节点设为“红色”。
			(03) 对x的父节点进行左旋。
			(04) 左旋后，重新设置x的兄弟节点。
		Case 2 	x是“黑+黑”节点，x的兄弟节点是黑色，x的兄弟节点的两个孩子都是黑色。 	
			(01) 将x的兄弟节点设为“红色”。
			(02) 设置“x的父节点”为“新的x节点”。
		Case 3 	x是“黑+黑”节点，x的兄弟节点是黑色；x的兄弟节点的左孩子是红色，右孩子是黑色的。 	
			(01) 将x兄弟节点的左孩子设为“黑色”。
			(02) 将x兄弟节点设为“红色”。
			(03) 对x的兄弟节点进行右旋。
			(04) 右旋后，重新设置x的兄弟节点。
		Case 4 	x是“黑+黑”节点，x的兄弟节点是黑色；x的兄弟节点的右孩子是红色的，x的兄弟节点的左孩子任意颜色。 	
			(01) 将x父节点颜色 赋值给 x的兄弟节点。
			(02) 将x父节点设为“黑色”。
			(03) 将x兄弟节点的右子节设为“黑色”。
			(04) 对x的父节点进行左旋。
			(05) 设置“x”为“根节点”。
	1. (Case 1)x是"黑+黑"节点，x的兄弟节点是红色
		1.1 现象说明
			x是"黑+黑"节点，x的兄弟节点是红色。(此时x的父节点和x的兄弟节点的子节点都是黑节点)。
		1.2 处理策略
			(01) 将x的兄弟节点设为“黑色”。
			(02) 将x的父节点设为“红色”。
			(03) 对x的父节点进行左旋。
			(04) 左旋后，重新设置x的兄弟节点。
			下面谈谈为什么要这样处理。(建议理解的时候，通过rbtree_delete_case1.jpg图进行对比)
			这样做的目的是将“Case 1”转换为“Case 2”、“Case 3”或“Case 4”，从而进行进一步的处理。对x的父节点进行左旋；左旋后，为了保持红黑树特性，就需要在左旋前“将x的兄弟节点设为黑色”，同时“将x的父节点设为红色”；左旋后，由于x的兄弟节点发生了变化，需要更新x的兄弟节点，从而进行后续处理。
	2. (Case 2) x是"黑+黑"节点，x的兄弟节点是黑色，x的兄弟节点的两个孩子都是黑色
		2.1 现象说明
			x是“黑+黑”节点，x的兄弟节点是黑色，x的兄弟节点的两个孩子都是黑色。
		2.2 处理策略
			(01) 将x的兄弟节点设为“红色”。
			(02) 设置“x的父节点”为“新的x节点”。
			下面谈谈为什么要这样处理。(建议理解的时候，通过rbtree_delete_case2.jpg图进行对比)
			这个情况的处理思想：是将“x中多余的一个黑色属性上移(往根方向移动)”。 x是“黑+黑”节点，我们将x由“黑+黑”节点 变成 “黑”节点，多余的一个“黑”属性移到x的父节点中，即x的父节点多出了一个黑属性(若x的父节点原先是“黑”，则此时变成了“黑+黑”；若x的父节点原先时“红”，则此时变成了“红+黑”)。 此时，需要注意的是：所有经过x的分支中黑节点个数没变化；但是，所有经过x的兄弟节点的分支中黑色节点的个数增加了1(因为x的父节点多了一个黑色属性)！为了解决这个问题，我们需要将“所有经过x的兄弟节点的分支中黑色节点的个数减1”即可，那么就可以通过“将x的兄弟节点由黑色变成红色”来实现。
			经过上面的步骤(将x的兄弟节点设为红色)，多余的一个颜色属性(黑色)已经跑到x的父节点中。我们需要将x的父节点设为“新的x节点”进行处理。若“新的x节点”是“黑+红”，直接将“新的x节点”设为黑色，即可完全解决该问题；若“新的x节点”是“黑+黑”，则需要对“新的x节点”进行进一步处理。
	3. (Case 3)x是“黑+黑”节点，x的兄弟节点是黑色；x的兄弟节点的左孩子是红色，右孩子是黑色的
		3.1 现象说明
			x是“黑+黑”节点，x的兄弟节点是黑色；x的兄弟节点的左孩子是红色，右孩子是黑色的。
		3.2 处理策略
			(01) 将x兄弟节点的左孩子设为“黑色”。
			(02) 将x兄弟节点设为“红色”。
			(03) 对x的兄弟节点进行右旋。
			(04) 右旋后，重新设置x的兄弟节点。
			下面谈谈为什么要这样处理。(建议理解的时候，通过rbtree_delete_case3.jpg图进行对比)
			我们处理“Case 3”的目的是为了将“Case 3”进行转换，转换成“Case 4”,从而进行进一步的处理。转换的方式是对x的兄弟节点进行右旋；为了保证右旋后，它仍然是红黑树，就需要在右旋前“将x的兄弟节点的左孩子设为黑色”，同时“将x的兄弟节点设为红色”；右旋后，由于x的兄弟节点发生了变化，需要更新x的兄弟节点，从而进行后续处理。
	4. (Case 4)x是“黑+黑”节点，x的兄弟节点是黑色；x的兄弟节点的右孩子是红色的，x的兄弟节点的左孩子任意颜色

		4.1 现象说明
			x是“黑+黑”节点，x的兄弟节点是黑色；x的兄弟节点的右孩子是红色的，x的兄弟节点的左孩子任意颜色。
		4.2 处理策略
			(01) 将x父节点颜色 赋值给 x的兄弟节点。
			(02) 将x父节点设为“黑色”。
			(03) 将x兄弟节点的右子节设为“黑色”。
			(04) 对x的父节点进行左旋。
			(05) 设置“x”为“根节点”。
			下面谈谈为什么要这样处理。(建议理解的时候，通过rbtree_delete_case4.jpg图进行对比)
			我们处理“Case 4”的目的是：去掉x中额外的黑色，将x变成单独的黑色。处理的方式是“：进行颜色修改，然后对x的父节点进行左旋。下面，我们来分析是如何实现的。
			为了便于说明，我们设置“当前节点”为S(Original Son)，“兄弟节点”为B(Brother)，“兄弟节点的左孩子”为BLS(Brother's Left Son)，“兄弟节点的右孩子”为BRS(Brother's Right Son)，“父节点”为F(Father)。
			我们要对F进行左旋。但在左旋前，我们需要调换F和B的颜色，并设置BRS为黑色。为什么需要这里处理呢？因为左旋后，F和BLS是父子关系，而我们已知BL是红色，如果F是红色，则违背了“特性(4)”；为了解决这一问题，我们将“F设置为黑色”。 但是，F设置为黑色之后，为了保证满足“特性(5)”，即为了保证左旋之后：
			第一，“同时经过根节点和S的分支的黑色节点个数不变”。
             若满足“第一”，只需要S丢弃它多余的颜色即可。因为S的颜色是“黑+黑”，而左旋后“同时经过根节点和S的分支的黑色节点个数”增加了1；现在，只需将S由“黑+黑”变成单独的“黑”节点，即可满足“第一”。
			第二，“同时经过根节点和BLS的分支的黑色节点数不变”。
             若满足“第二”，只需要将“F的原始颜色”赋值给B即可。之前，我们已经将“F设置为黑色”(即，将B的颜色"黑色"，赋值给了F)。至此，我们算是调换了F和B的颜色。
			第三，“同时经过根节点和BRS的分支的黑色节点数不变”。
             在“第二”已经满足的情况下，若要满足“第三”，只需要将BRS设置为“黑色”即可。
			经过，上面的处理之后。红黑树的特性全部得到的满足！接着，我们将x设为根节点，就可以跳出while循环(参考伪代码)；即完成了全部处理。
			至此，我们就完成了Case 4的处理。理解Case 4的核心，是了解如何“去掉当前节点额外的黑色”。
 */