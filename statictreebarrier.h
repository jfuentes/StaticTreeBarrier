/**
* Static Tree Barrier with atomic operations
* author: Joel Fuentes - joel.fuentes@uci.edu
**/

#include <atomic>
#include <vector>

/* Class Static Tree Barrier with atomic operations*/
class StaticTreeBarrier {

 public:
    /* Node class */
    class Node{

    public:

      //constructor
      Node(Node *parent, int count, StaticTreeBarrier *mybarrier){
         children_=count;
         mybarrier_=mybarrier;
         parent_=parent;
         atomic_init(&child_count_, 0);
         child_count_.store(children_, std::memory_order_release);
      }


      void await(){
         bool mysense = mybarrier_->thread_sense_.load(std::memory_order_acquire);
         printf("\nheere %d", children_);
         while(child_count_.load(std::memory_order_acquire)>0) {
            thrd_yield();
         }; /* spin until children done */
         printf("\nheere");
         child_count_.store(children_, std::memory_order_release); /* prepare for next round */
         if(parent_ != NULL){ // not root
            parent_->childDone();  //indicate child subtree completion
            while(mybarrier_->sense_ != mysense) {
               thrd_yield();
            };  //wait for global sense to change
         }else{
            // I am root: toggle global sense
            mybarrier_->thread_sense_ .store(!mybarrier_->thread_sense_.load(std::memory_order_acquire), std::memory_order_release);
         }
         //toggle sense
         mybarrier_->thread_sense_.store(!mysense, std::memory_order_release);
      }

      void childDone(){
         //atomic_store_explicit(&ptr->lock, UNLOCKED, memory_order_release);
         child_count_.fetch_sub(1);
      }

   private:
      /* Number of children */
      unsigned int children_;

      /* reference to the parent */
      Node *parent_;

      /* Number of children incomplete */
      std::atomic<int> child_count_;

      /*reference to the Barrier*/
      StaticTreeBarrier *mybarrier_;
    };

    //constructor
	 StaticTreeBarrier (int n, int radix) : n_(n), radix_(radix) {
      nodes_ = 0;
      //arr_nodes_.resize(n_);
      unsigned int depth = 0;

      /* compute the depth */
      while(n > 1){
         depth++;
         n = n / radix;
      }

      build( NULL, depth);
      sense_ = false;
      atomic_init(&thread_sense_ , !sense_);
	}


   /* recursive tree constructor */
   void build(Node *parent, int depth){
   // are we at a leaf node?
    if (depth == 0) {
      //arr_nodes_[nodes_++] = new Node(parent, 0);
      arr_nodes_.push_back( Node(parent, 0, this));
      nodes_++;
    } else {
      Node myNode = Node(parent, radix_, this);
      //arr_nodes_[nodes_++] = myNode;
      arr_nodes_.push_back(myNode);
      nodes_++;
      for (int i = 0; i < radix_; i++) {
        build(&myNode, depth - 1);
      }
    }
   }

   //this version needs the thread id as a parameter
	void await(int thread_id) {
		//thrd_t t = thrd_current();
      //unsigned int thread_id = t.priv->get_id();
      arr_nodes_[thread_id].await();
	}

   //variables

public:
   /* thread-local sense*/
   std::atomic<bool>  thread_sense_;
   bool sense_; // global sense

private:
   /* Number of synchronized threads. */
	int n_;

   /* Radix */
   int radix_;

   /* used to build the tree */
   int nodes_;

   /* Array of nodes */
   std::vector<Node> arr_nodes_;
};
