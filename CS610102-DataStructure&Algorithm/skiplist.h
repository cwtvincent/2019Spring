#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <algorithm>

using namespace std;
const int INTMAX = 2147483647;
const int INTMIN = -2147483648;

struct node{
    int val;
    node *back, *next, *up, *down;
    node() {val = 0; back = NULL; next = NULL; up = NULL; down = NULL;} 
    node(int e) {val = e; back = NULL; next = NULL; up = NULL; down = NULL;}   
};

class skiplist
{    
    public:
        skiplist() {}
        bool insr(int e);
        bool dele(int e);
        bool closest(int e, int &ret);
        node* find(int e);  
        node* find(int e, int h); 
        void view();   
        void init();

    private:    
        vector<node*> head, end;
        unsigned int headlen = 0;
};	

void skiplist::init(){
    node *a, *b;
    a = new node(INTMIN);
    b = new node(INTMAX);
    a->next = b;
    b->back = a;
    head.clear();
    end.clear();
    head.push_back(a);
    end.push_back(b);
    headlen = 2;
}

bool skiplist::closest(int e, int &ret){
    node *vhead;
    
    vhead = find(e);    
    if(vhead->next->val == INTMAX)
        return false;
    
    ret = vhead->next->val;
    return true;
}

void skiplist::view(){
    node *h;
    int i = 0, len = head.size(), a = 0;
    
    if(headlen == 0){cout << "empty skip list\n";}
    else{
        for(i = len-1; i != -1; i--){
            h = head[i];
            h = h->next;
            if(h->val != INTMAX){
                cout << "\n  level" << i << ": ";                
                cout << h->val << ' ';
                while(h->next && h->next->val != INTMAX){                        
                    h = h->next;
                    cout << h->val << ' ';
                }
            }
            else{
                a++;
            }
        }
        cout << "\nmaxheight = " << head.size()-a << '\n';
    }
    cout << '\n';
}

node* skiplist::find(int e){ 
    node *vhead;
    
    vhead = head.back();
    while(vhead->down){
        while(vhead->next && vhead->next->val <= e)
            vhead = vhead->next;
        vhead = vhead->down;        
    }
    while(vhead->next && vhead->next->val <= e)
        vhead = vhead->next;
    return vhead;
}

node* skiplist::find(int e, int h){
    int level = head.size();    
    node *vhead;
    
    vhead = head.back();
    while(vhead->down && level > h){
        while(vhead->next && vhead->next->val <= e)
            vhead = vhead->next;
        vhead = vhead->down;        
        level--;
    }
    while(vhead->next && vhead->next->val <= e)
        vhead = vhead->next;
    return vhead;
}

bool skiplist::insr(int e){
    node *vhead, *nhead, *c;
    node *a, *b;
    unsigned int h = 1;

    vhead = find(e);
    //if(vhead->val == e) return false;
    headlen++;
    nhead = new node(e);    
    vhead->next->back = nhead;
    nhead->next = vhead->next;
    vhead->next = nhead; 
    nhead->back = vhead;  

    //grow
    
    while(h + 2 < headlen){                
        if(rand() % 2 == 1){
            c = nhead;
            nhead = new node(e);
            h++;
            if(head.size() < h){                
                a = new node(INTMIN);
                b = new node(INTMAX);
                a->next = nhead;
                b->back = nhead;
                nhead->back = a;
                nhead->next = b;
                a->down = head.back();
                head.back()->up = a;
                b->down = end.back();
                end.back()->up = b;
                nhead->down = c;
                c->up = nhead;
                head.push_back(a);
                end.push_back(b);
            }
            else{
                vhead = find(e, h);  
                vhead->next->back = nhead;
                nhead->next = vhead->next;
                vhead->next = nhead; 
                nhead->back = vhead;
                nhead->down = c;
                c->up = nhead;
            }              
        }
        else
            break; 
    }

    return true;
}

bool skiplist::dele(int e){
    node *n;

    n = find(e);
    if(n->val != e){
        return false;        
    }
    n->next->back = n->back;
    n->back->next = n->next;
    while(n->up){
        n = n->up;
        n->next->back = n->back;
        n->back->next = n->next;
    }
    headlen--;
    return true;
}

