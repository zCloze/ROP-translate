#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

using namespace std;

int hex2int(string hex_value) {
  stringstream ss;
  int rvalue;
  ss << hex << hex_value;
  ss >> rvalue;

  return rvalue;
}

string int2hex(int value) {
  stringstream ss;
  ss << hex << value;

  return ss.str();
}

struct Block
{
  string disas;
  int last;
  int offset = -1;
  static Block empty() {
    return Block { "", -1 };
  }
  bool isEmpty() {
    return ((disas == "") && (last = -1));
  }
};

vector<Block> _580blocks;
vector<Block> _991blocks;

void generate(string filename, vector<Block>* blocks) {
  ifstream file(filename);
  string context;

  string disas;
  string addr;
  Block block;
  while (getline(file, context)) {
    int find = context.find(':');

    int rvalue = hex2int(addr);
    if (context == "end") {
      if (disas.length() > 4) {
        rvalue += (int)(disas.length() / 4);
      }

      block.last = rvalue;
      blocks->push_back(block);

      block = Block::empty();
    }
    else if (find != -1) {
      // disas
      addr = context.substr(0, 5);
      disas = context.substr(5, context.find(':') - 5);

      if (disas.length() > 4) {
        for (int i = 4; i < disas.length(); i++) {
          disas[i] = '0';
        }
      }

      block.disas += disas;
    }
  }
}

string getChain(vector<Block> blocks, int addr) {
  for (Block block : blocks) {
    int offset = block.last - addr;;
    if (offset > 0 && offset < (int)(block.disas.length() / 2)) {
      return block.disas.substr(block.disas.length() - 4 - (offset * 2), offset * 2 + 4);
    }
  }
  return "\0";
}

tuple<string, int> getFunctionChain(vector<Block> blocks, int addr) {
  for (Block block : blocks) {
    int offset = block.last - addr;
    if (offset > 0 && offset < (int)(block.disas.length() / 2)) {
      return tuple<string, int>(block.disas, offset);
    }
  }
  return tuple<string, int>("", -1);
}

// function vnx2cnx ready

Block translateFunction(vector<Block> blocksf, vector<Block> blockss, int addr) {
  auto [func, offset] = getFunctionChain(blocksf, addr);
  for (Block block : blockss) {
    if (block.disas == func) {
      block.offset = offset;
      return block;
    }
  }
  return Block::empty();
}

int translateByFunction(vector<Block> blocksf, vector<Block> blockss, int addr) {
  Block block = translateFunction(blocksf, blockss, addr);
  if (block.isEmpty()) return -1;

  return block.last - block.offset;
}

vector<int> translateByChain(vector<Block> blocksf, vector<Block> blockss, int addr) {
  vector<int> rAddrs;
  string chain = getChain(blocksf, addr);

  for (Block block : blockss) {
    for (int i = 0; i < block.disas.length(); i += 4) {
      if (block.disas.substr(i, block.disas.length() - i) == chain) {
        int begin = block.last - (block.disas.length() / 2) + 2;
        rAddrs.push_back(begin + i / 2);
      }
    }
  }

  return rAddrs;
}

int closest(vector<int> vect, int key) {
  int temp = key;
  int rvalue;

  for (int i : vect) {
    if (abs(i - key) < temp) {
      temp = abs(i - key);
      rvalue = i;
    }
  }

  return rvalue;
}

int main() {
  generate("./assets/fx580vnx_disas.txt", &_580blocks);
  generate("./assets/fx991cnx_disas.txt", &_991blocks);

  while (true) {
    string _;
    int width = 40;

    cout << "[580 or 991 disas [exit]] > ";
    cin >> _;

    if (_ == "exit") return 0;
    int disas = hex2int(_);

    cout << "[580 -> 991]:" << endl;

    string best = " [best choice]";
    vector<int> chains = translateByChain(_580blocks, _991blocks, disas);
    int best_choice = translateByFunction(_580blocks, _991blocks, disas);
    if (best_choice == -1) {
      best_choice = closest(chains, disas);
      best = " [closest choice]";
    }
    
    for (int raddr : chains) {
      stringstream stream;
      stream << "    " << int2hex(disas) << " -> " << int2hex(raddr);

      if (raddr == best_choice) stream << best;
      cout << stream.str() << endl;
    }

    //////////////////////////////////////////////////////////////////////

    cout << "[991 -> 580]:" << endl;

    best = " [best choice]";
    chains = translateByChain(_991blocks, _580blocks, disas);
    best_choice = translateByFunction(_991blocks, _580blocks, disas);
    if (best_choice == -1) {
      best_choice = closest(chains, disas);
      best = " [closest choice]";
    }
    
    for (int raddr : chains) {
      stringstream stream;
      stream << "    " << int2hex(disas) << " -> " << int2hex(raddr);

      if (raddr == best_choice) stream << best;
      cout << stream.str() << endl;
    }

    cout << "==================================" << endl << endl;
  }
  return 0;
}