#include <atomic>
#include <vector>

class static_tree_barrier {


 public:
    class Node{
    public:
      Node(const Node &parent, int count) : children_(count){
         *parent_=parent;
         child_count_=children_;
      }

      void await(){
         bool mysense = thread_sense_;
         while(child_count_.load()>0) {}; /* spin until children done */
         child_count_ = children_;  /* prepare for next round */
         if(parent_ != NULL){ // not root
            parent_->childDone();  //indicate child subtree completion
            while(sense_ != mysense) {};  //wait for global sense to change
         }else{
            sense_ = !sense_; // I am root: toggle global sense
         }
         thread_sense_ = !mysense;  //toggle sense

      }

      void childDone(){
         child_count_.fetch_sub(1);
      }


      /* Number of children */
      unsigned int children_;

      /* reference to the parent */
      Node *parent_;

      /* Number of children incomplete */
      std::atomic<unsigned int> child_count_;
   };

	static_tree_barrier (unsigned int n, unsigned int radix) : n_(n), radix_(radix) {
      nodes_ = 0;
      //arr_nodes_.resize(n_);
      unsigned int depth = 0;

      /* compute the depth */
      while(n > 1){
         depth++;
         n = n / radix;
      }

      //build( (Node)NULL, depth);
      sense_ = false;
      thread_sense_ = !sense_;
	}


   /* recursive tree constructor */
   void build(Node parent, int depth){
   // are we at a leaf node?
    if (depth == 0) {
      //arr_nodes_[nodes_++] = new Node(parent, 0);
      arr_nodes_.push_back( Node(parent, 0));
      nodes_++;
    } else {
      Node myNode = Node(parent, radix_);
      //arr_nodes_[nodes_++] = myNode;
      arr_nodes_.push_back(myNode);
      nodes_++;
      for (int i = 0; i < radix_; i++) {
        build(myNode, depth - 1);
      }
    }
   }

	void await(int thread_id) {
		//thrd_t t = thrd_current();
      //unsigned int thread_id = t.priv->get_id();
      arr_nodes_[thread_id].await();
	}

 protected:
	/* Number of synchronized threads. */
	unsigned int n_;

   /* Radix */
   unsigned radix_;

   /* Sense */
   static bool sense_;



   /* thread-local sense*/
   //thread_local bool thread_sense_;
   static bool thread_sense_;

   /* used to build the tree */
   unsigned int nodes_;


private:
   /* Array of nodes */
   std::vector<Node> arr_nodes_;

};
