/**
* Static Tree Barrier with atomic operations
* author: Joel Fuentes - joel.fuentes@uci.edu
**/

#include <stdatomic.h>
#include <vector>
#include <model-assert.h>

using namespace std;
//#define DEBUG

/* Class Static Tree Barrier with atomic operations*/
class StaticTreeBarrier {

 public:
    /* Node class */
    class Node{

    public:

      //constructor
      Node(Node *parent, int count, StaticTreeBarrier *mybarrier){
         atomic_init(&child_count_, count);
         #ifdef DEBUG
         printf("\nNode created %d children", atomic_load_explicit(&child_count_, memory_order_relaxed));
         #endif
         children_=count;
         mybarrier_=mybarrier;
         parent_=parent;

      }


      void await(){
         //bool mysense = atomic_load(&mybarrier_->thread_sense_);
         bool mysense = mybarrier_->thread_sense_;
         #ifdef DEBUG
         printf("\nCurrent children %d", children_);
         #endif
         while(1){
            if(atomic_load_explicit(&child_count_, memory_order_acquire)==0) {
               break;
            }
               thrd_yield();
         } /* spin until children done */

         atomic_store_explicit(&child_count_, children_, memory_order_relaxed); /* prepare for next round */
         if(parent_ != NULL){ // not root
            parent_->childDone();  //indicate child subtree completion
            //while(atomic_load(&mybarrier_->sense_) != mysense) {
            while(mybarrier_->sense_ != mysense) {
               thrd_yield();
            };  //wait for global sense to change
         }else{
            // am root: toggle global sense
            //atomic_store(&mybarrier_->sense_, mysense);
            mybarrier_->sense_=mysense;
         }
         //toggle sense
         //atomic_store(&mybarrier_->thread_sense_, !mysense);
         mybarrier_->thread_sense_=!mysense;
      }

      void childDone(){
         //atomic_store_explicit(&ptr->lock, UNLOCKED, memory_order_release);
         atomic_fetch_sub_explicit(&child_count_, 1, memory_order_release);
      }

   private:
      /* Number of children */
      int children_;

      /* reference to the parent */
      Node *parent_;

      /* Number of children incomplete */
      atomic_int child_count_;

      /*reference to the Barrier*/
      StaticTreeBarrier *mybarrier_;
    };

    //constructor
	 StaticTreeBarrier (int n, int radix) : n_(n), radix_(radix) {
      nodes_ = 0;
      //arr_nodes_.resize(n_);
      int depth = 0;

      /* compute the depth */
      while(n > 1){
         depth++;
         n = n / radix_;
      }

      build( NULL, depth);

      MODEL_ASSERT(arr_nodes_.size()==n_);
      MODEL_ASSERT(nodes_==n_);

      sense_=false;
      thread_sense_=true;
      //atomic_init(&sense_, false);
      //atomic_init(&thread_sense_ , true);
	}


   /* recursive tree constructor */
   void build(Node *parent, int depth){
   // are we at a leaf node?
    if (depth == 0) {
      Node *myNode =new Node(parent, 0, this);
      arr_nodes_.push_back( myNode);
      nodes_++;
    } else {
      Node *myNode = new Node(parent, radix_, this);
      arr_nodes_.push_back(myNode);
      nodes_++;
      for (int i = 0; i < radix_; i++) {
        build(myNode, depth - 1);
      }
    }
   }

   //this version needs the thread id as a parameter
	void await(int thread_id) {
      arr_nodes_[thread_id]->await();
	}



public:
   /* thread-local sense*/
   //atomic_bool thread_sense_;
   //atomic_bool sense_; // global sense
   bool sense_, thread_sense_;
private:
   /* Number of synchronized threads. */
	int n_;

   /* Radix */
   int radix_;

   /* used to build the tree */
   int nodes_;

   /* Array of nodes */
   std::vector<Node*> arr_nodes_;
};
