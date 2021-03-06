package main

import (
	"fmt"
)

/*
	多个defer的执行顺序为后进先出
	defer return 返回值的执行逻辑是
	return最先执行，
	return负责将结果写入返回值中，
	接着defer开始执行收尾工作
	最后函数携带当前返回值退出
	https://blog.csdn.net/qq_22063697/article/details/74892728
*/
/*
	先执行非defer代码
	将值赋给return的i，
	再defer出栈
	return值返回函数
*/
func test_01() int {
	fmt.Println("test_01")
	i := 0
	defer fmt.Println("defer1 i:", i) // 7:0
	fmt.Println("body1 i:", i)        // 1 ：0
	defer func() {
		i++                             // 5
		fmt.Println("defer func i:", i) // 6：2
	}()
	i++                               //2
	defer fmt.Println("defer2 i:", i) // 4：1
	fmt.Println("body2 i:", i)        // 3:1
	return i
}

/*
	先非defer代码
	再defer出栈
	最后panic
*/
func test_02() int {
	fmt.Println("test_02")
	i := 0
	defer fmt.Println("defer1 i:", i) // 7:0
	fmt.Println("body1 i:", i)        // 1 ：0
	defer func() {
		i++                             // 5
		fmt.Println("defer func i:", i) // 6：2
	}()
	i++                               //2
	defer fmt.Println("defer2 i:", i) // 4：1
	fmt.Println("body2 i:", i)        // 3:1
	panic("123")                      // 8  在所有的defer之后执行
}
func main() {
	fmt.Println("main1 i:", test_01()) // 8：1
	test_02()
}
