#include <vector>
#include <map>
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

vector<Block> _580vnx_blocks;
vector<Block> _880btg_blocks;
vector<Block> _991cnx_blocks;
vector<Block> _991cncw_blocks;
vector<Block> _jp900n_blocks;
vector<Block> _570es_blocks;
vector<Block> _570es_plus_blocks;
map<string, vector<Block>> blocks_map;

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

void init() {
  generate("./assets/fx580vnx_disas.txt", &_580vnx_blocks);
  generate("./assets/fx880btg_disas.txt", &_880btg_blocks);
  generate("./assets/fx991cnx_disas.txt", &_991cnx_blocks);
  generate("./assets/fx991cncw_disas.txt", &_991cncw_blocks);
  generate("./assets/fxjp900n_disas.txt", &_jp900n_blocks);
  generate("./assets/fx570es_disas.txt", &_570es_blocks);
  generate("./assets/fx570esplus_disas.txt", &_570es_plus_blocks);

  blocks_map = {
    { "580 VNX", _580vnx_blocks },
    { "880 BTG", _880btg_blocks },
    { "991 CNX", _991cnx_blocks },
    { "991 CNCW", _991cncw_blocks },
    { "JP900 N", _jp900n_blocks },
    { "570 ES", _570es_blocks },
    { "570 ES PLUS", _570es_plus_blocks}
  };
}

int main(int argc, char* argv[]) {
  init();

  vector<string> arguments(argv + 1, argv + argc);
  for (string disas : arguments) {
    for (const auto& kvx : blocks_map) {
      string name_x = kvx.first;
      vector<Block> blocks_x = kvx.second;

      for (const auto& kvy : blocks_map) {
        string name_y = kvy.first;
        vector<Block> blocks_y = kvy.second;

        if (name_x == name_y) continue;

        string best_str = " \x1b[92m[best option]\x1b[0m";
        vector<int> traned_disas = translateByChain(blocks_x, blocks_y, hex2int(disas));

        int best_option = translateByFunction(blocks_x, blocks_y, hex2int(disas));
        if (best_option == -1) {
          best_option = closest(traned_disas, hex2int(disas));
          best_str = " \x1b[92m[closest option]\x1b[0m";
        }

        if (traned_disas.size() != 0 ) {
          cout << "\x1b[93m" << "[     " << name_x << " -> " << name_y << "     ]" << "\x1b[0m" << endl;
          for (int dis : traned_disas) {
            stringstream stream;
            stream << "\t" << disas << " -> " << int2hex(dis);
            if (best_option == dis) stream << best_str;

            cout << stream.str() << endl;
          }
        }
      }
    }
  }
  
  return 0;
}
