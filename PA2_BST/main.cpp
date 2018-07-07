#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <map>
#include <vector>
#include <utility>

using namespace std;

struct Node {
	int value;
	Node *parent;
	Node *left_child;
	Node *right_child;
};

void bst_insert(Node *node, Node *to_insert) {
	if (to_insert->value < node->value) {
		// check if left_child is empty
		if (!node->left_child) {
			// left_child is empty, insert directly
			// doubly-linked
			node->left_child = to_insert;
			to_insert->parent = node;
		} else {
			// left_child is used, call bst_insert again
			bst_insert(node->left_child, to_insert);
		}
	} else if (to_insert->value > node->value) {
		// check if right_child is empty
		if (!node->right_child) {
			// right_child is empty, insert directly
			// doubly-linked
			node->right_child = to_insert;
			to_insert->parent = node;
		} else {
			// right_child is used, call bst_insert again
			bst_insert(node->right_child, to_insert);
		}
	} else {
		cerr << "Error: same value nodes are inserted: " << to_insert->value << "!" << endl;
	}
}

string print_par(Node *root) { // return string in order to use for both cout and fout
	string par_rep;
	par_rep += to_string(root->value);

	if (!root->left_child && !root->right_child) {
		return par_rep; // terminate printing, so no need for else
	}

	// at least left or right child exists
	par_rep += "(";

	if (root->left_child) { // left child exists
		par_rep += print_par(root->left_child);
	} else { // left child is empty
		par_rep += "-";
	}

	// deal with spaces
	// if the last element in the string is ")", no need to print spaces
	// coz that's when there are children for its left child
	if (par_rep.back() != ')') { 
		par_rep += " "; // 
	}

	if (root->right_child) { // right child exists
		par_rep += print_par(root->right_child);
	} else { // right child is empty
		par_rep += "-";
	}

	// at least left or right child exists
	par_rep += ")";

	return par_rep;
}

map<Node*, int> node_height(Node *root, int height) {
	// depth-first search over the BST
	// record the height for every node
	map<Node*, int> node_h;
	node_h.insert(pair<Node*, int>(root, height)); // insert root

	if (root->left_child) { // left child exists
		map<Node*, int> l_node_h = node_height(root->left_child, height + 1);

		for (auto it = l_node_h.begin(); it != l_node_h.end(); it++){
			node_h.insert(*it);
		}
	}

	if (root->right_child) { // right child exists
		map<Node*, int> r_node_h = node_height(root->right_child, height + 1);

		for (auto it = r_node_h.begin(); it != r_node_h.end(); it++){
			node_h.insert(*it);
		}
	}

	return node_h;
}

Node* getleftmost(Node *leftmost) {
	if (leftmost->left_child) { 
		// left child exists, then go to its left
		return getleftmost(leftmost->left_child); 
	} else { 
		// there is no left child, i.e. it is already the desired leftmost
		return leftmost;
	}
}

map<Node*, int> node_order(Node *node, int order) { // in-order traversal over the BST
	// x_location does not overlap
	// record the order for every node
	map<Node*, int> node_o;

	// if it doesn't exist, terminate the function
	if (!node) return node_o; 
	// otherwise
	node_o.insert(pair<Node*, int>(node, order));

	if (node->right_child) { // right subtree exists
		// goto and start with the leftmost node
		Node *leftmost = getleftmost(node->right_child);
		map<Node*, int> next_node_o = node_order(leftmost, order + 1); // recursively save
		for (auto it = next_node_o.begin(); it != next_node_o.end(); it++){
			node_o.insert(*it);
		}

	} else { // find parent of left child
		// initialize
		Node *curr = node;
		Node *next = node->parent;
		// loop and change curr and next, until found
		while (next && curr == next->right_child) { 
			// current's parent exists and current is its right child
			curr = next; // move upwards, parent be the new node
			next = next->parent;  			
		} // out of loop when current is its parent's left child 
		map<Node*, int> next_node_o = node_order(next, order + 1); // recursively save	
		for (auto it = next_node_o.begin(); it != next_node_o.end(); it++){
			node_o.insert(*it);
		}
	}

	return node_o;
}

string tex_print(map<Node*, int> node_h, map<Node*, int> node_o) {
	map<pair<int, int>, Node*> tex;
	int max_y = 0;
	
	for (auto it = node_h.begin(); it != node_h.end(); it++) { // choose one map to iterate through
		Node *n = it->first; // the node we get
		int h = it->second; // get height
		if (h > max_y) max_y = h;
		int o = node_o[n]; // get order
		pair<int, int> coor = make_pair(o, h); // pair up its x, y coordinates
		tex[coor] = n; // insert into map tex
	}

	string result;

	for (int y = 0; y <= max_y; y++) {
		for (int x = 0; x < node_o.size(); x++) {
			Node *n = tex[make_pair(x, y)];
			if (!n) { // that location has no node
				result += "  "; // print two spaces
			} else {
				result += to_string(n->value);
			}
		}
		result += "\n"; // finished this row of x coordinates
	}

	return result;
}

vector<Node*> l_bound(Node *root) { 
	// depth-first search over the BST
	// record the leftmost node

	vector<Node*> left_b; // format: left[depth] = node;
	left_b.push_back(root); // left_b[0] = root;
	
	if (root->left_child) { // left child exists
		vector<Node*> l_left_b = l_bound(root->left_child); // for left subtree
		// push all elements in left subtree into left boundary
		for (auto it = l_left_b.begin(); it != l_left_b.end(); it++) {
			left_b.push_back(*it); 
		}
	} 

	if (root->right_child) { // right child exists
		vector<Node*> r_right_b = l_bound(root->right_child); // for right subtree
		// when the right subtree is deeper than the left subtree		
		if (r_right_b.size() > left_b.size() - 1) { // 1 more element than l_left_b with root
			int offset = left_b.size() - 1;
			for (auto it = r_right_b.begin() + offset; it != r_right_b.end(); it++) {
				left_b.push_back(*it); // elements in right subtree should be pushed as well
			}
		}
	}

	return left_b;
}

int main(int argc, char* argv[]) {

	// ./bst test_case.txt
	// ./bst tree.txt	

	ifstream fin(argv[1]); // <input_file_name>
	ofstream fpar;
	fpar.open("parenthesis_representation.txt");
	ofstream ftex; 
	ftex.open("textual_printing.txt");
	ofstream fleft; 	
	fleft.open("left_boundary.txt");

	string tree;

	while(getline(fin, tree)) {
		// content into ss
		// tree is released and to be further reused

		stringstream ss(tree); 

		int temp;
		Node *root = new Node(); // initialize BST
		ss >> root->value;
		// cout << root->value << " "; // debug for input file readin
		
		while (ss >> temp) {
			Node *node = new Node();
			node->value = temp;
			// cout << temp << " "; // debug for input file readin
			bst_insert(root, node);
		}

		// cout << endl; // debug for input file readin

		// added to reuse
		ss.clear(); 
    	ss.str("");	

    	// parenthesis_representation.txt

		fpar << print_par(root) << endl;
		
			/*if (!root->value) {
				// no root exists
				cerr << "Error: no BST built to be printed!";
			} else {
	    		cout << print_par(root) << endl;
	    	}*/

		// textual_printing.txt
		Node *leftmost = getleftmost(root);
    	string tex = tex_print(node_height(root, 0), node_order(leftmost, 0));	
    	ftex << tex << endl;
	    	// cout << tex;

			/*Node *leftmost_of_tree = getleftmost(root);
			cout << "Leftmost of tree: " << leftmost_of_tree->value << endl;

			map<Node*, int> node_x = node_order(leftmost_of_tree, 0);
	    	for (auto it = node_x.begin(); it != node_x.end(); it++) {
				cout << (*it).first->value << " => " << (*it).second << endl;
			}
			cout << endl;*/


	    	/*map<Node*, int> node_y = node_height(root, 0);
	    	for (auto it = node_y.begin(); it != node_y.end(); it++) {
				cout << (*it).first->value << " => " << (*it).second << endl;
			}
			cout << endl;*/

    	// left_boundary.txt
    		
		vector<Node*> left_bound = l_bound(root);
    	for (auto it = left_bound.begin(); it != left_bound.end(); it++) {
    		fleft << (*it)->value;
    		if (it + 1 != left_bound.end()) fleft << " ";
    	}
    	fleft << endl;
		
	    	/*vector<Node*> left_bound = l_bound(root);
	    	for (auto it = left_bound.begin(); it != left_bound.end(); it++) {
	    		cout << (*it)->value << " ";
	    	}
	    	cout << endl;*/
	}

	fin.close();
	fpar.close();
	ftex.close();
	fleft.close();

	return 0;
}