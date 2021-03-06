# map介绍
1. map数据类型是键值形式的hash表，一一映射，可以快速查找添加删除。遍历返回的顺序随机
2. map是引用类型，和指针切片一样，声明后指向nil，如下使用是错误的
    ```
    var m map[string]string //只声明，没有分配内存，指向nil
    m["result"] = "result" // 会出错
    ```
    正确的做法
    ```
    m := make(map[string]string)
    m["result"] = "result"
3. map不是并发安全的，存在map并发写入错误 `参见example.go：test_02()`
    * 解决方法1：通过对map进行加读写锁实现并发map
    * 解决方法2:: `参见example.go：test_03()`使用sync.Map，实现了锁机制，保证了并发安全，提供了Store，Load，Delete，Range等操作，开箱可用（即声明后可直接使用）
# 参考资料
[慎用golang中的map，特别是在并发操作中](https://studygolang.com/articles/17099)