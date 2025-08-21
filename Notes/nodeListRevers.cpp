#include <cstddef>
#include <iostream>
#include <unordered_set>

using namespace std;


class Node {
public:
    int val;
    Node* next;

    explicit Node(int v) : val(v),next(nullptr){};
    Node():Node(0) {}; 
    Node(int v, Node* n):val(v),next(n) {};



    Node& operator=(const Node& node) {
        if (this != &node) {
            this->val = node.val;
            this->next = node.next;
        }
        return *this;
    };

    Node& operator=(Node&& node) {
        if (this != &node) {
            this->val = node.val;
            this->next = node.next;
            node.next = nullptr;
            node.val = 0;
        }

        return *this;
    }
};


class NodeList {
public:
    Node* head;
    Node* tail;
    int length;

    NodeList() {
        head = nullptr;
        tail = nullptr;
        length = 0;
    };

    explicit NodeList(int len) {
        this->length = len;
        if (length == 0 )
            return;

        this->head = new Node(0);

        Node* cur = head;
        for(int i = 1; i < length; i++) {
            cur->next = new Node(i);
            cur = cur->next;
        }
        this->tail = cur;
        
        return;
    }

       // 拷贝赋值运算符
    NodeList& operator=(const NodeList& other) {
        if (this != &other) {
            // 释放当前资源
            Node* current = head;
            while (current != nullptr) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            
            // 复制新资源
            length = other.length;
            if (length == 0) {
                head = nullptr;
                tail = nullptr;
                return *this;
            }

            head = new Node(other.head->val);
            Node* newCurrent = head;
            Node* otherCurrent = other.head->next;
            
            while (otherCurrent != nullptr) {
                newCurrent->next = new Node(otherCurrent->val);
                newCurrent = newCurrent->next;
                otherCurrent = otherCurrent->next;
            }
            
            tail = newCurrent;
        }
        return *this;
    }


    // 移动赋值运算符
    NodeList& operator=(NodeList&& other) noexcept {
        if (this != &other) {
            // 释放当前资源
            Node* current = head;
            while (current != nullptr) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            
            // 转移资源
            head = other.head;
            tail = other.tail;
            length = other.length;
            other.head = nullptr;
            other.tail = nullptr;
            other.length = 0;
        }
        return *this;
    }

    // 迭代版本的反转（优化原版本）
    void Reverse() {
        Node* cur = head;
        Node* pre = nullptr;

        while(cur != nullptr) {
            Node* nex = cur->next;
            cur->next = pre;
            pre = cur;
            cur = nex;
        }
        Node* tmp = head;
        head = tail;
        tail = tmp;
    }

    Node* reverseFromHeadHelper(Node* node) {
        if(node == nullptr || node->next == nullptr)
            return node;
        
        Node* rest = reverseFromHeadHelper(node->next);
        node->next->next = node;
        node->next = nullptr;
        
        return rest;
    }

    void ReverseFromHead() {
        if(head == nullptr || head->next == nullptr) {
            return;
        }
        Node* tmp = head;
        head = reverseFromHeadHelper(head);
        tail = tmp;
        tail->next  = nullptr;
    }

    // 递归版本的链表反转
    void ReverseRecursive() {
        if (head == nullptr || head->next == nullptr) {
            return; // 空链表或只有一个节点，无需反转
        }
        
        Node* tmp = head;
        head = reverseRecursiveHelper(head);
        tail = tmp;
        tail->next = nullptr; // 确保尾节点的next为nullptr
    }

private:
    // 递归辅助函数
    Node* reverseRecursiveHelper(Node* node) {
        if (node == nullptr || node->next == nullptr) {
            return node; // 基本情况：空节点或最后一个节点
        }
        
        Node* newHead = reverseRecursiveHelper(node->next);
        node->next->next = node; // 反转指针
        node->next = nullptr; // 断开原来的指针
        
        return newHead;
    }

public:
    // 自检链表的反转（带错误检查）
    bool ReverseWithCheck() {
        // 检查链表是否为空
        if (head == nullptr) {
            cout << "错误：链表为空，无法反转" << endl;
            return false;
        }
        
        // 检查链表是否形成环
        if (hasCycle()) {
            cout << "错误：链表中存在环，无法反转" << endl;
            return false;
        }
        
        // 检查链表长度是否匹配
        if (!checkLength()) {
            cout << "警告：链表长度与记录不符" << endl;
        }
        
        // 执行反转
        Node* cur = head;
        Node* pre = nullptr;
        int count = 0;
        
        while(cur != nullptr) {
            Node* nex = cur->next;
            cur->next = pre;
            pre = cur;
            cur = nex;
            count++;
        }
        
        Node* tmp = head;
        head = tail;
        tail = tmp;
        
        // 反转后再次检查长度
        if (count != length) {
            cout << "警告：反转后链表长度与记录不符" << endl;
            return false;
        }
        
        return true;
    }

private:
    // 检查链表是否有环
    bool hasCycle() {
        if (head == nullptr || head->next == nullptr) {
            return false;
        }
        
        Node* slow = head;
        Node* fast = head->next;
        
        while (fast != nullptr && fast->next != nullptr) {
            if (slow == fast) {
                return true;
            }
            slow = slow->next;
            fast = fast->next->next;
        }
        
        return false;
    }
    
    // 检查链表长度是否匹配
    bool checkLength() {
        int count = 0;
        Node* cur = head;
        
        while (cur != nullptr) {
            count++;
            cur = cur->next;
        }
        
        return count == length;
    }

public:
    void Print() {
        Node* cur = head;
        while(cur != nullptr){
            std::cout << cur->val << " ";
            cur = cur->next;
        }

        std::cout<<endl;
    }

    // 删除链表中的重复元素
    void RemoveDuplicates() {
        if (head == nullptr || head->next == nullptr) {
            return; // 空链表或只有一个节点，无需处理
        }
        
        unordered_set<int> seenValues;
        Node* current = head;
        Node* prev = nullptr;
        
        while (current != nullptr) {
            if (seenValues.find(current->val) != seenValues.end()) {
                // 发现重复值，删除当前节点
                prev->next = current->next;
                delete current;
                current = prev->next;
                length--; // 更新链表长度
                
                // 如果删除的是尾节点，更新尾指针
                if (current == nullptr) {
                    tail = prev;
                }
            } else {
                // 未发现重复值，添加到集合中
                seenValues.insert(current->val);
                prev = current;
                current = current->next;
            }
        }
    }

    // 删除链表中的重复元素（不使用额外空间，双重循环）
    void RemoveDuplicatesNoExtraSpace() {
        if (head == nullptr || head->next == nullptr) {
            return; // 空链表或只有一个节点，无需处理
        }
        
        Node* current = head;
        
        while (current != nullptr) {
            Node* runner = current;
            while (runner->next != nullptr) {
                if (runner->next->val == current->val) {
                    // 发现重复值，删除节点
                    Node* temp = runner->next;
                    runner->next = runner->next->next;
                    delete temp;
                    length--; // 更新链表长度
                    
                    // 如果删除的是尾节点，更新尾指针
                    if (runner->next == nullptr) {
                        tail = runner;
                    }
                } else {
                    runner = runner->next;
                }
            }
            current = current->next;
        }
    }


};


int main() {
    cout << "=== 测试链表反转的三种方法 ===" << endl;
    
    // 测试迭代版本的反转
    cout << "\n1. 测试迭代版本的反转：" << endl;
    NodeList list1(5);
    cout << "原始链表：";
    list1.Print();
    list1.Reverse();
    cout << "反转后链表：";
    list1.Print();
    
    // 测试递归版本的反转
    cout << "\n2. 测试递归版本的反转：" << endl;
    NodeList list2(5);
    cout << "原始链表：";
    list2.Print();
    list2.ReverseRecursive();
    cout << "反转后链表：";
    list2.Print();
    
    // 测试自检链表的反转
    cout << "\n3. 测试自检链表的反转：" << endl;
    NodeList list3(5);
    cout << "原始链表：";
    list3.Print();
    bool success = list3.ReverseWithCheck();
    if (success) {
        cout << "反转后链表：";
        list3.Print();
    } else {
        cout << "反转失败" << endl;
    }
    
    // 测试边界情况：空链表
    cout << "\n4. 测试边界情况 - 空链表：" << endl;
    NodeList emptyList;
    cout << "空链表自检反转：";
    emptyList.ReverseWithCheck();
    
    // 测试边界情况：单节点链表
    cout << "\n5. 测试边界情况 - 单节点链表：" << endl;
    NodeList singleList(1);
    cout << "单节点链表：";
    singleList.Print();
    singleList.ReverseRecursive();
    cout << "递归反转后：";
    singleList.Print();
    
    cout << "\n=== 测试删除链表重复元素功能 ===" << endl;
    
    // 测试删除重复元素（使用哈希集合）
    cout << "\n6. 测试删除重复元素（使用哈希集合）：" << endl;
    NodeList dupList1;
    // 手动创建带有重复元素的链表: 1->2->3->2->4->1->5
    dupList1.head = new Node(1);
    dupList1.head->next = new Node(2);
    dupList1.head->next->next = new Node(3);
    dupList1.head->next->next->next = new Node(2);
    dupList1.head->next->next->next->next = new Node(4);
    dupList1.head->next->next->next->next->next = new Node(1);
    dupList1.head->next->next->next->next->next->next = new Node(5);
    dupList1.tail = dupList1.head->next->next->next->next->next->next;
    dupList1.length = 7;
    
    cout << "原始链表：";
    dupList1.Print();
    dupList1.RemoveDuplicates();
    cout << "删除重复元素后链表：";
    dupList1.Print();
    cout << "链表长度：" << dupList1.length << endl;
    
    // 测试删除重复元素（不使用额外空间）
    cout << "\n7. 测试删除重复元素（不使用额外空间）：" << endl;
    NodeList dupList2;
    // 手动创建带有重复元素的链表: 1->2->3->2->4->1->5
    dupList2.head = new Node(1);
    dupList2.head->next = new Node(2);
    dupList2.head->next->next = new Node(3);
    dupList2.head->next->next->next = new Node(2);
    dupList2.head->next->next->next->next = new Node(4);
    dupList2.head->next->next->next->next->next = new Node(1);
    dupList2.head->next->next->next->next->next->next = new Node(5);
    dupList2.tail = dupList2.head->next->next->next->next->next->next;
    dupList2.length = 7;
    
    cout << "原始链表：";
    dupList2.Print();
    dupList2.RemoveDuplicatesNoExtraSpace();
    cout << "删除重复元素后链表：";
    dupList2.Print();
    cout << "链表长度：" << dupList2.length << endl;
    
    // 测试边界情况：空链表删除重复元素
    cout << "\n8. 测试边界情况 - 空链表删除重复元素：" << endl;
    NodeList emptyDupList;
    cout << "空链表删除重复元素：";
    emptyDupList.RemoveDuplicates();
    emptyDupList.Print();
    
    // 测试边界情况：单节点链表删除重复元素
    cout << "\n9. 测试边界情况 - 单节点链表删除重复元素：" << endl;
    NodeList singleDupList;
    singleDupList.head = new Node(1);
    singleDupList.tail = singleDupList.head;
    singleDupList.length = 1;
    cout << "单节点链表：";
    singleDupList.Print();
    singleDupList.RemoveDuplicates();
    cout << "删除重复元素后：";
    singleDupList.Print();
    
    // 测试所有元素都相同的情况
    cout << "\n10. 测试所有元素都相同的情况：" << endl;
    NodeList allSameList;
    allSameList.head = new Node(5);
    allSameList.head->next = new Node(5);
    allSameList.head->next->next = new Node(5);
    allSameList.head->next->next->next = new Node(5);
    allSameList.tail = allSameList.head->next->next->next;
    allSameList.length = 4;
    cout << "所有元素相同的链表：";
    allSameList.Print();
    allSameList.RemoveDuplicates();
    cout << "删除重复元素后：";
    allSameList.Print();
    cout << "链表长度：" << allSameList.length << endl;
    
    return 0;
}