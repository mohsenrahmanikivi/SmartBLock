
#include <_doublesha256.h>
#include <_messageParser.h>
#include <_rootFromProof.h>

static int nextFlagIs = 0;
static int nextHashIs = 0;

  string _rootFromProof(char* bin_flags,int current_depth,int tree_depth, string* hash_list,string tx_in_block)
  {






      int temp_flag=  bin_flags[nextFlagIs] - 48 ;
     // cout<<nextFlagIs<<"temp_flag = "<<temp_flag<<"\r\n";
      nextFlagIs++;

      if(temp_flag== 0) {
    	  nextHashIs++;
        return hash_list[nextHashIs-1];
      } else if(temp_flag == 1 && current_depth == tree_depth) {
          nextHashIs++;
          return tx_in_block; // we must use hash_list[hashCount-1] but we jump from the the hash and put tx instead
        } else {
            string  left = _rootFromProof(bin_flags, current_depth+1, tree_depth, hash_list, tx_in_block);
            string right = _rootFromProof(bin_flags, current_depth+1, tree_depth, hash_list, tx_in_block);
            return _doublesha256(left.append(right));
        		}

  return ("True");


  }

  void Reset (){
	  nextFlagIs = 0;
	  nextHashIs = 0;
  }

