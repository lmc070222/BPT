#ifndef Bplustree_HPP
#define Bplustree_HPP
#include "/home/lmc123456/Ticket-System-2026/BPT/src/vector.hpp"
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>

template <typename KeyType, typename ValueType, int M> class Bplustree {
  struct FileHeader {
    uint32_t root_page;
    uint32_t leaf_head;
  };
#pragma pack(push, 1)
  struct NodePage {
    bool is_leaf;
    int key_num;
    KeyType keys[M + 2];
    uint32_t parent = 0;
    union {
      ValueType values[M + 2];
      uint32_t children[M + 2];
    };
    uint32_t next = 0;
  };
#pragma pack(pop)
  class FileManager {
  public:
    std::fstream s;
    FileManager(const std::string &filename) {
      if (!s.is_open()) {
        s.open(filename, std::ios::out | std::ios::binary);
        s.close();
        s.open(filename, std::ios::in | std::ios::out | std::ios::binary);
      }
    }
    void ReadPage(uint32_t page_no, NodePage *buf) {
      s.seekg(page_no * sizeof(NodePage) + sizeof(FileHeader));
      s.read(reinterpret_cast<char *>(buf), sizeof(NodePage));
    }
    void WritePage(uint32_t page_no, const NodePage *buf) {
      s.seekp(page_no * sizeof(NodePage) + sizeof(FileHeader));
      s.write(reinterpret_cast<char *>(buf), sizeof(NodePage));
    }
    uint32_t AllocPage(const NodePage *buf) {
      s.seekp(0, std::ios::end);
      uint32_t new_page = (s.tellp()) / sizeof(NodePage) + 1;
      s.write(reinterpret_cast<char *>(buf), sizeof(NodePage));
      return new_page;
    }
    void ReadHeader(FileHeader *buf) {
      s.seekg(0);
      s.read(reinterpret_cast<char *>(buf), sizeof(FileHeader));
    }
    void WriteHeader(const FileHeader *buf) {
      s.seekp(0);
      s.write(reinterpret_cast<char *>(buf), sizeof(FileHeader));
    }
  };
  const std::string name = "data";
  FileManager fm;
  uint32_t root_page_;
  uint32_t leaf_head_;
  Bplustree() : fm(name), root_page_(0), leaf_head_(0) {
    fm.s.seekg(0, std::ios::end);
    auto fileSize = fm.s.tellg();
    if (fileSize < sizeof(FileHeader)) {
        FileHeader init{0, 0};
        fm.WriteHeader(&init);
    } else {
        FileHeader header;
        fm.ReadHeader(&header);
        root_page_ = header.root_page;
        leaf_head_ = header.leaf_head;
    }
}
  void SplitCommon(uint32_t page_no, NodePage &curnode) {
    NodePage new_leaf;
    int t = 0;
    for (int i = M / 2; i < curnode.key_num; i++) {
      new_leaf.keys[t] = curnode.keys[i];
      new_leaf.children[t] = curnode.children[i];
      t++;
    }
    curnode.key_num = M / 2;
    new_leaf.key_num = t;
    new_leaf.parent = curnode.parent;

    new_leaf.next = curnode.next;

    curnode.next = fm.AllocPage(new_leaf);

    fm.WritePage(page_no, curnode);
    InsertIntoParent(page_no, new_leaf.keys[0], curnode.next);
  }
  void InsertIntoParent(uint32_t leftPageNo, KeyType key,
                        uint32_t rightPageNo) {
    NodePage leftnode;
    NodePage rightnode;
    fm.ReadPage(leftPageNo, &leftnode);
    fm.ReadPage(rightPageNo, &rightnode);
    if (leftnode.parent == 0) {
      NodePage new_parent;
      new_parent.parent = 0;
      new_parent.is_leaf = false;
      new_parent.key_num = 1;
      new_parent.keys[0] = key;
      new_parent.children[0] = leftPageNo;
      new_parent.children[1] = rightPageNo;
      leftnode.parent = fm.AllocPage(&new_parent);
      rightnode.parent = leftnode.parent;
      FileHeader fileheader;
      fm.ReadHeader(&fileheader);
      fileheader.root_page = leftnode.parent;
      fm.WriteHeader(&fileheader);
      fm.WritePage(leftPageNo, &leftnode);
      fm.WritePage(rightPageNo, &rightnode);
      return;
    }
    uint32_t parentno = leftnode.parent;
    NodePage parent;
    fm.ReadPage(parentno, &parent);
    int k = parent.key_num;
    ;
    for (int i = 0; i < parent.key_num; i++) {
      if (parent.keys[i] >= key) {
        k = i;
        break;
      }
    }
    for (int i = parent.key_num - 1; i >= k; i--) {
      parent.keys[i + 1] = parent.keys[i];
      parent.children[i + 1] = parent.children[i];
    }
    parent.keys[k] = key;
    parent.children[k] = rightPageNo;
    parent.key_num++;
    fm.WritePage(parentno, &parent);
    if (parent.key_num <= M)
      return;
    SplitCommon(parentno, parent);
  }
  void SplitLeaf(uint32_t page_no, NodePage &curnode) {
    NodePage new_leaf;
    int t = 0;
    for (int i = M / 2; i < curnode.key_num; i++) {
      new_leaf.keys[t] = curnode.keys[i];
      new_leaf.values[t] = curnode.values[i];
      t++;
    }
    curnode.key_num = M / 2;
    new_leaf.key_num = t;
    new_leaf.parent = curnode.parent;

    new_leaf.next = curnode.next;

    curnode.next = fm.AllocPage(new_leaf);

    fm.WritePage(page_no, curnode);
    InsertIntoParent(page_no, new_leaf.keys[0], curnode.next);
  }
  void Insert(KeyType &index, ValueType &value) {
    if (leaf_head_ == 0) {
      NodePage new_;
      new_.is_leaf = true;
      new_.key_num = 1;
      new_.keys[0] = index;
      new_.values[0] = value;
      new_.parent = 0;
      new_.next = 0;
      uint32_t new_page = fm.AllocPage(&new_);
      FileHeader new_header;
      new_header.root_page = new_header.leaf_head = new_page;
      root_page_ = leaf_head_ = new_page;
      fm.WriteHeader(&new_header);
      return;
    } else {
      NodePage curnode;
      uint32_t x = root_page_;
      while (true) {
        fm.ReadPage(x, &curnode);
        if (curnode.is_leaf == true)
          break;
        else {
          int i = 0;
          for (; i < curnode.key_num; i++) {
            if (curnode.keys[i] >= index)
              break;
          }
          x = curnode.children[i];
        }
      }
      int k = curnode.key_num;
      for (int i = 0; i < curnode.key_num; i++) {
        if (curnode.keys[i] >= index) {
          k = i;
          break;
        }
      }

      for (int j = curnode.key_num - 1; j >= k;j--) {
        curnode.keys[j + 1] = curnode.keys[j];
        curnode.values[j + 1] = curnode.values[j];
      }
      curnode.key_num++;
      curnode.keys[k] = index;
      curnode.values[k] = value;
      fm.WritePage(x, &curnode);
      if (curnode.key_num <= M)
        return;
      else {
        SplitLeaf(x, curnode);
      }
    }
  }
  void sibling(uint32_t curno, uint32_t afterno) {
    NodePage curnode;
    NodePage afternode;
    fm.ReadPage(curno, &curnode);
    fm.ReadPage(afterno, &afternode);
    NodePage parentnode;
    fm.ReadPage(curnode.parent, &parentnode);
    if (curnode.is_leaf == true) {
      int t = 0;
      for (int i = curnode.key_num; i < curnode.key_num + afternode.key_num;
           i++) {
        curnode.keys[i] = afternode.keys[t];
        curnode.values[i] = afternode.values[t];
        t++;
      }
    } else {
      int t = 0;
      for (int i = curnode.key_num; i < curnode.key_num + afternode.key_num;
           i++) {
        curnode.keys[i] = afternode.keys[t];
        curnode.children[i] = afternode.children[t];
        t++;
      }
    }
    curnode.key_num = curnode.key_num + afternode.key_num;
    int cur, after;
    for (int i = 0; i < parentnode.key_num; i++) {
      if (parentnode.children[i] == curno)
        cur = i;
      if (parentnode.children[i] == afterno)
        after = i;
    }
    if (cur < after)
      curnode.next = afternode.next;
    else {
      if (after != 0) {
        NodePage pa;
        fm.ReadPage(parentnode.children[after - 1], &pa);
        pa.next = curno;
        fm.WritePage(parentnode.children[after - 1], &pa);
      }
    }
    for (int i = after; i < parentnode.key_num - 1; i++) {
      parentnode.keys[i] = parentnode.keys[i + 1];
      parentnode.children[i] = parentnode.children[i + 1];
    }
    parentnode.key_num--;
    if (curnode.parent == root_page_) {
      curnode.parent = 0;
      root_page_ = curno;
    }
    fm.WritePage(curnode.parent, &parentnode);
    fm.WritePage(curnode, &curnode);
    if (parentnode.key_num >= M / 2)
      return;
    balancedelete(curnode.parent);
  }
  void balancedelete(uint32_t pageno) { // 删除节点低于M/2后维护节点平衡
    // 此处应该处理根节点失去平衡的情况
    NodePage curnode;
    NodePage parentnode;
    NodePage leftnode;
    NodePage rightnode;
    fm.ReadPage(pageno, &curnode);
    uint32_t parentno = curnode.parent;
    fm.ReadPage(parentno, &parentnode);
    int k;
    for (int i = 0; i < parentnode.key_num; i++) {
      if (parentnode.children[i] == pageno) {
        k = i;
        break;
      }
    }
    if (k != 0) {
      fm.ReadPage(parentnode.children[k - 1], &leftnode);
      if (leftnode.key_num > M / 2) {  // 向左借一个节点
        if (curnode.is_leaf == true) { // 叶子节点
          for (int i = 0; i < curnode.key_num; i++) {
            curnode.keys[i + 1] = curnode.keys[i];
            curnode.values[i + 1] = curnode.values[i];
          }
          curnode.keys[0] = leftnode.keys[leftnode.key_num - 1];
          curnode.values[0] = leftnode.keys[leftnode.key_num - 1];
          curnode.key_num++;
          leftnode.key_num--;
          parentnode.keys[k - 1] = leftnode.keys[leftnode.key_num];
          fm.WritePage(pageno, &curnode);
          fm.WritePage(parentno, &parentnode);
          fm.WritePage(parentnode.children[k - 1], &leftnode);
          return;
        }
        if (curnode.is_leaf == false) { // 非叶子节点
          for (int i = 0; i < curnode.key_num; i++) {
            curnode.keys[i + 1] = curnode.keys[i];
            curnode.children[i + 1] = curnode.children[i];
          }
          curnode.keys[0] = parentnode.keys[k - 1];
          curnode.children[0] = parentnode.children[k - 1];
          parentnode.keys[k - 1] = leftnode.keys[leftnode.key_num];
          parentnode.children[k - 1] = leftnode.children[leftnode.key_num];
          curnode.key_num++;
          leftnode.key_num--;
          fm.WritePage(pageno, &curnode);
          fm.WritePage(parentno, &parentnode);
          fm.WritePage(parentnode.children[k - 1], &leftnode);
          return;
        }
      }
    }
    if (k != parentnode.key_num - 1) {
      fm.ReadPage(parentnode.children[k + 1], &rightnode);
      if (rightnode.key_num > M / 2) { // 向右借一个节点
        if (curnode.is_leaf == true) { // 叶子节点
          curnode.keys[curnode.key_num] = rightnode.keys[0];
          curnode.values[curnode.key_num] = rightnode.keys[0];
          for (int i = 0; i < curnode.key_num - 1; i++) {
            rightnode.keys[i] = rightnode.keys[i + 1];
            rightnode.values[i] = rightnode.values[i + 1];
          }
          curnode.key_num++;
          rightnode.key_num--;
          parentnode.keys[k] = curnode.keys[curnode.key_num - 1];
          fm.WritePage(pageno, &curnode);
          fm.WritePage(parentno, &parentnode);
          fm.WritePage(parentnode.children[k - 1], &leftnode);
          return;
        }
        if (curnode.is_leaf == false) { // 非叶子节点
          curnode.keys[curnode.key_num] = parentnode.keys[k];
          curnode.children[curnode] = parentnode.children[k];
          parentnode.keys[k] = rightnode.keys[0];
          parentnode.children[k] = rightnode.children[0];
          for (int i = 0; i < curnode.key_num - 1; i++) {
            rightnode.keys[i] = curnode.keys[i + 1];
            rightnode.children[i] = rightnode.children[i + 1];
          }
          curnode.key_num++;
          rightnode.key_num--;
          fm.WritePage(pageno, &curnode);
          fm.WritePage(parentno, &parentnode);
          fm.WritePage(parentnode.children[k + 1], &rightnode);
          return;
        }
      }
    } else {
      if (k != 0) {
        sibling(pageno, parentnode.children[k - 1]);
        return;
      }
      if (k != parentnode.key_num - 1) {
        sibling(parentnode.children[k + 1], pageno);
        return;
      }
    }
  }
  void search(uint32_t nextpageno, KeyType &index,
              ValueType &value) { // 在nextpageno中寻找键值对
    NodePage nextpage;
    fm.ReadPage(nextpageno, &nextpage);
    int k = nextpage.key_num;
    for (int i = 0; i < nextpage.key_num; i++) {
      if (nextpage.keys[i] >= index) {
        k = i;
        break;
      }
    }
    if (nextpage.key_num == k)
      return;
    while (true) {
      if (nextpage.keys[k] > index)
        return;
      if (nextpage.keys[k] == index) {
        if (nextpage.values[k] == value) {
          nextpage.key_num--;
          for (int i = k; i < nextpage.key_num; i++) {
            nextpage.keys[i] = nextpage.keys[i + 1];
          }
          fm.WritePage(nextpageno, &nextpage);
          if (nextpage.key_num < M / 2) {
            balancedelete(nextpageno);
          }
          return;
        }
      }
      k++;
      if (k >= nextpage.key_num) {
        search(nextpage.next, index, value);
        return;
      }
    }
  }
  void deletenode(KeyType &index, ValueType &value) {
    NodePage curnode;
    uint32_t x = root_page_;
    while (true) {
      fm.ReadPage(x, &curnode);
      if (curnode.is_leaf == true)
        break;
      else {
        int i = 0;
        for (; i < curnode.key_num; i++) {
          if (curnode.keys[i] >= index)
            break;
        }
        x = curnode.children[i];
      }
    }
    int k = curnode.key_num;
    for (int i = 0; i < curnode.key_num; i++) {
      if (curnode.keys[i] >= index) {
        k = i;
        break;
      }
    }
    if (curnode.key_num == k)
      return;
    while (true) {
      if (curnode.keys[k] > index)
        return;
      if (curnode.keys[k] == index) {
        if (curnode.values[k] == value) {
          curnode.key_num--;
          for (int i = k; i < curnode.key_num; i++) {
            curnode.keys[i] = curnode.keys[i + 1];
          }
          fm.WritePage(x, &curnode);
          if (curnode.key_num < M / 2) {
            balancedelete(x);
          }
          return;
        }
      }
      k++;
      if (k >= curnode.key_num) {
        search(curnode.next, index, value);
        return;
      }
    }
  }
  sjtu::vector<ValueType> find(KeyType key) {
    sjtu::vector<ValueType> ans;
    NodePage curnode;
    uint32_t x = root_page_;
    while (true) {
      fm.ReadPage(x, &curnode);
      if (curnode.is_leaf == true)
        break;
      else {
        int i = 0;
        for (; i < curnode.key_num; i++) {
          if (curnode.keys[i] >= key)
            break;
        }
        x = curnode.children[i];
      }
    }
    while (true) {
      for (int i = 0; i < curnode.key_num; i++) {
        if (curnode.keys[i] == key)
          ans.push_back(curnode.values[i]);
      }
      if (curnode.keys[curnode.key_num - 1] != key or curnode.next == 0) return ans;
     fm.ReadPage(curnode.next,&curnode);
    }
  }
};

#endif