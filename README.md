# CGTL (C Generic Template Library)

参考C++的STL(Standard Template Library),

``` Bash
include/
├── cgtl.h                  # 容器头文件配置文件
序列式容器（Sequence Containers）
├── array.h                 # 固定大小数组容器
├── vector.h                # 动态数组: 支持快速随机访问 内存连续
├── forward_list.h          # 单向链表: 只能顺序遍历，但不支持随机访问
├── list.h                  # 双向链表: 只能顺序遍历，但不支持随机访问
├── deque.h                 # 双端队列: 支持快速随机访问 支持首尾快速插入/删除，内存不连续
关联式容器（Associative Containers）
├── set.h                   # 集合  :不允许重复元素
├── multiset.h              # 多集合: 允许重复元素
├── map.h                   # 映射: 每个键映射到一个值
├── multimap.h              # 多重映射:存储了键值对(pair),允许唯一键映射到多个值
无序关联式容器（Unordered Associative Containers）
├── unorder_set.h           # 无序集合
├── unordered_multiset.h    # 无序多重集合
├── unorder_map.h           # 无序映射
├── unordered_multimap.h    # 无序多重映射
容器适配器（Container Adapters）
├── stack.h                 # 栈
├── queue.h                 # 队列
├── pri_queue.h             # 优先队列
辅助容器（Auxiliary Containers）
├── str.h                   # 字符串
└── README.md
```

```bash

                    vec  str  deq  lst  set  pqu  que  stk  ust
+-------------------------------------------------------------+
empty               x    x    x    x    x    x    x    x    x
each                x    x    x    x    x                   x
equal               x    x    x    x    x    x    x    x    x
swap                x    x    x    x    x    x    x    x    x
bucket                                                      x
bucket_size                                                 x
load_factor                                                 x
rehash                                                      x
insert              x    x    x    x    x                   x
init                x    x    x    x    x    x    x    x    x
free                x    x    x    x    x    x    x    x    x
step                x    x    x    x    x                   x
range               x    x    x    x    x                   x
find                x    x    x    x    x                   x
count                    x              x                   x
lower_bound                             x                   
upper_bound                             x                   
erase               x    x    x    x    x                   x
copy                x    x    x    x    x                   x
begin               x    x    x    x    x                   x
end                 x    x    x    x    x                   x
intersection                            x                   
union                                   x                   
difference                              x                   
symmetric_difference                    x                   
top                                          x         x
push                                         x    x    x
pop                                          x    x    x
at                  x    x    x
front               x    x    x    x              x
back                x    x    x    x              x
set                 x    x    x
pop_back            x    x    x    x
pop_front                     x    x
clear               x    x    x    x    x
reserve             x    x
push_back           x    x    x    x
push_front                    x    x
transfer                           x
disconnect                         x
connect                            x
resize              x    x    x    x
assign              x    x    x    x
reverse                            x
shrink_to_fit       x    x
data                x    x
erase_node                              x
sort                x    x    x    x
remove_if           x    x    x    x    x
splice                             x
merge                              x
unique                             x
append                   x
insert_str               x
replace                  x              x
c_str                    x
find                     x
rfind                    x
find_first_of            x
find_last_of             x
find_first_not_of        x
find_last_not_of         x
substr                   x
compare                  x
key_compare              x
```

# 使用
```c 
/**
 * @file vector_demo.c 
 */
/********** 使用某个类型 **********/
#define T int       // [必选] 模板类型选择
#include "vector.h" // 最后引入需要引入的stl

// 具体的算法步骤

/********** 使用另外某个类型 **********/
typedef struct Person{
    int id;
    char *name;
} Person;
#define T Person       // [必选] 模板类型选择
#include "vector.h" // 最后引入需要引入的stl
// 如果需要实现释放内存，在源文件实现 VEC_free 函数
void free_person(Person *p){
    free(p->name);
}

// 具体的算法步骤

// /********** 在同一文件下不能重复定义某个类型 **********/
// #define T int       // 这里 int 之前已经定义过了就不能在定义了
// #include "vector.h" 

/********** 使用某个类型的不同容器 **********/
#define T int       // 这里 int 是给deque用的不是 vector
#include "deque.h" 
```


