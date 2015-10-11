#include <atomic>
#include <vector>


class StaticTreeBarrier {
 public:
    class Node{
    public:
      Node(Node *parent, int count, StaticTreeBarrier *mybarrier) : children_(count){
         mybarrier_=mybarrier;
         parent_=parent;
         child_count_=children_;
      }

      void await(){
         bool mysense = mybarrier_->thread_sense_;
         while(child_count_.load()>0) {}; /* spin until children done */
         child_count_ = children_;  /* prepare for next round */
         if(parent_ != NULL){ // not root
            parent_->childDone();  //indicate child subtree completion
            while(mybarrier_->sense_ != mysense) {};  //wait for global sense to change
         }else{
            mybarrier_->sense_ = !mybarrier_->sense_; // I am root: toggle global sense
         }
         mybarrier_->thread_sense_ = !mysense;  //toggle sense

      }

      void childDone(){
         child_count_.fetch_sub(1);
      }

    protected:
      /* Number of children */
      unsigned int children_;

      /* reference to the parent */
      Node *parent_;

      /* Number of children incomplete */
      std::atomic<unsigned int> child_count_;

      /*reference to the Barrier*/
      StaticTreeBarrier *mybarrier_;
    };

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
      thread_sense_ = !sense_;
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

	void await(int thread_id) {
		//thrd_t t = thrd_current();
      //unsigned int thread_id = t.priv->get_id();
      arr_nodes_[thread_id].await();
	}
/* thread-local sense*/
bool  thread_sense_;
bool sense_; // global sense
 protected:
	/* Number of synchronized threads. */
	int n_;

   /* Radix */
   int radix_;





   /* used to build the tree */
   int nodes_;


private:
   /* Array of nodes */
   std::vector<Node> arr_nodes_;




};
