#include <iostream>

using namespace std;

// 链表节点定义
class Node {
public:
    int val;
    Node* next;

    explicit Node(int v) : val(v), next(nullptr) {};
    Node() : Node(0) {}; 
    Node(int v, Node* n) : val(v), next(n) {};
};

// 合并两个有序链表的函数
Node* mergeTwoSortedLists(Node* l1, Node* l2) {
    // 处理边界情况：如果一个链表为空，直接返回另一个链表
    if (l1 == nullptr) {
        return l2;
    }
    if (l2 == nullptr) {
        return l1;
    }
    
    // 创建一个虚拟头节点，简化代码逻辑
    Node dummy(0);
    Node* current = &dummy;
    
    // 遍历两个链表，比较节点值
    while (l1 != nullptr && l2 != nullptr) {
        if (l1->val <= l2->val) {
            current->next = l1;
            l1 = l1->next;
        } else {
            current->next = l2;
            l2 = l2->next;
        }
        current = current->next;
    }
    
    // 将剩余的节点连接到合并后的链表
    if (l1 != nullptr) {
        current->next = l1;
    } else {
        current->next = l2;
    }
    
    return dummy.next;
}

// 辅助函数：创建有序链表
Node* createSortedList(int arr[], int size) {
    if (size == 0) {
        return nullptr;
    }
    
    Node* head = new Node(arr[0]);
    Node* current = head;
    
    for (int i = 1; i < size; i++) {
        current->next = new Node(arr[i]);
        current = current->next;
    }
    
    return head;
}

// 辅助函数：打印链表
void printList(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        cout << current->val << " ";
        current = current->next;
    }
    cout << endl;
}

// 辅助函数：释放链表内存
void freeList(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

// 测试函数
void testMergeTwoSortedLists() {
    cout << "=== 测试合并两个有序链表 ===" << endl;
    
    // 测试1：两个非空链表
    cout << "\n测试1：两个非空链表" << endl;
    int arr1[] = {1, 3, 5, 7};
    int arr2[] = {2, 4, 6, 8};
    Node* list1 = createSortedList(arr1, 4);
    Node* list2 = createSortedList(arr2, 4);
    
    cout << "链表1: ";
    printList(list1);
    cout << "链表2: ";
    printList(list2);
    
    Node* merged1 = mergeTwoSortedLists(list1, list2);
    cout << "合并后: ";
    printList(merged1);
    freeList(merged1);
    
    // 测试2：一个链表为空
    cout << "\n测试2：一个链表为空" << endl;
    int arr3[] = {1, 2, 3};
    Node* list3 = createSortedList(arr3, 3);
    Node* list4 = nullptr;
    
    cout << "链表3: ";
    printList(list3);
    cout << "链表4: 空链表" << endl;
    
    Node* merged2 = mergeTwoSortedLists(list3, list4);
    cout << "合并后: ";
    printList(merged2);
    freeList(merged2);
    
    // 测试3：两个链表长度不同
    cout << "\n测试3：两个链表长度不同" << endl;
    int arr5[] = {1, 3, 5, 7, 9};
    int arr6[] = {2, 4};
    Node* list5 = createSortedList(arr5, 5);
    Node* list6 = createSortedList(arr6, 2);
    
    cout << "链表5: ";
    printList(list5);
    cout << "链表6: ";
    printList(list6);
    
    Node* merged3 = mergeTwoSortedLists(list5, list6);
    cout << "合并后: ";
    printList(merged3);
    freeList(merged3);
    
    // 测试4：有重复元素的链表
    cout << "\n测试4：有重复元素的链表" << endl;
    int arr7[] = {1, 3, 3, 5};
    int arr8[] = {3, 4, 6};
    Node* list7 = createSortedList(arr7, 4);
    Node* list8 = createSortedList(arr8, 3);
    
    cout << "链表7: ";
    printList(list7);
    cout << "链表8: ";
    printList(list8);
    
    Node* merged4 = mergeTwoSortedLists(list7, list8);
    cout << "合并后: ";
    printList(merged4);
    freeList(merged4);
    
    // 测试5：两个空链表
    cout << "\n测试5：两个空链表" << endl;
    Node* list9 = nullptr;
    Node* list10 = nullptr;
    
    cout << "链表9: 空链表" << endl;
    cout << "链表10: 空链表" << endl;
    
    Node* merged5 = mergeTwoSortedLists(list9, list10);
    cout << "合并后: ";
    printList(merged5);
    freeList(merged5);
}

int main() {
    testMergeTwoSortedLists();
    return 0;
}