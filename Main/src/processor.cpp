#include "main_comp.hpp"
#include <time.h>

void Processor::init()
{
  srand((unsigned)time(0));
  for (int i = 1; i <= MAX_NUM_REGISTERS; i++)
  {
    registers.insert({"r" + to_string(i), 0});
  }
  registers.insert({"CMPR", 0});
  registers.insert({"RET_V", 0});
  
  for (int i = 0; i < assembly.size(); i++)
  {
    if (assembly[i]->type == JUMP_LABEL)
    {
      jump_labels.insert({assembly[i]->label, assembly[i]->counter});
    }
  }
}

void Processor::run()
{
  try
  {
    init();

    while (programCounter < assembly.size())
    {
      AssemblyEntry * instance = assembly[programCounter];

      if (programCounter == 72)
      {
          int asdf = 123;
      }

      switch (instance->type)
      {
        case JUMP_LABEL:
          break;
        case JUMP_OPP:
          if (jump_labels.find(instance->firstOp) == jump_labels.end() && instance->firstOp != "RET_A")
          {
            throw (int)ErrorInvalidSymbol; 
          }
          else if (instance->operatorLabel == "jc")
          {
            programCounter = registers["CMPR"] > 0 ? jump_labels[instance->firstOp] : programCounter;
          }
          else if (instance->operatorLabel == "jmp")
          {
            if (instance->firstOp == "MAIN")
            {
              isGettingGlobal = false;
            }
            programCounter = instance->firstOp == "RET_A" ? return_addresses.top() : jump_labels[instance->firstOp];
          }
          break;
        case ALU_OPP:
          readALUop(instance);
          break;
        default:
          throw (int)ErrorInvalidOpCode;
      }
      programCounter++;
    }
  }
  catch(int e)
  {
    ErrorReader::readError((ErrorCode)e, "");
  }
}

void Processor::readALUop(AssemblyEntry * assemblyLine)
{
  int left = 0;
  int right = 0;

  if (assemblyLine->operatorLabel == "load")
  {
    if (isGettingGlobal) 
    { 
      global_vars.insert({assemblyLine->firstOp, stoi(assemblyLine->secondOp)}); 
      return;
    }
    
    if (this->isLoadValid(assemblyLine->firstOp))
    {
      this->setValue(assemblyLine->firstOp, this->getValue(assemblyLine->secondOp));
    }
    else if (data_mem.find(assemblyLine->firstOp) == data_mem.end() && this->isArrayInst(assemblyLine->firstOp) == false) 
    {
      data_mem.insert({assemblyLine->firstOp, this->getValue(assemblyLine->secondOp)});
    }
    else
    {
      throw (int)ErrorInvalidLoad;
    }
  }
  else if (assemblyLine->operatorLabel == "arrL")
  {

    // TODO: 
    // - Have the PARAM_ARR be a list of strings to keep track what is being pushed.
    // - Once it hits a return then check if that PARAM_ARR list is 

    /*if (array_table.find(assemblyLine->firstOp) != array_table.end())
    {
        array_table[assemblyLine->firstOp].clear();
    }*/
    if (array_table.find(assemblyLine->secondOp) != array_table.end())
    {
      array_table[assemblyLine->firstOp] = array_table[assemblyLine->secondOp];
    }
    else if (assemblyLine->secondOp == "PARAM_ARR")
    {
      if (array_table.find(assemblyLine->firstOp) != array_table.end())
      {
          array_table[assemblyLine->firstOp] = array_table[assemblyLine->secondOp];
      }
      else
      {
          array_table.insert({ assemblyLine->firstOp, get<1>(params_array.front()) });
      }

      for (int index = 0; index < params_convert_count; index++)
      {
          string flag = "PARAM_ARR_" + index;
          for (auto it = params_convert.begin(); it != params_convert.end(); it++)
          {
              if (it->second == flag)
              {
                  it->second = assemblyLine->firstOp;
              }
          }
      }

      params_array.pop();
    }
    else
    {
      if (assemblyLine->firstOp == "RET_V" || assemblyLine->secondOp == "RET_V")
      {
        if (array_table.find("RET_V") == array_table.end())
        {
          array_table.insert({assemblyLine->firstOp, vector<int>()});
        }
        array_table[assemblyLine->firstOp] = array_table[assemblyLine->secondOp];
      }
      else if (this->getValue(assemblyLine->secondOp) == 0)
      {
        array_table.insert({assemblyLine->firstOp, vector<int>()});
      }
      else
      {
        array_table.insert({assemblyLine->firstOp, vector<int>(this->getValue(assemblyLine->secondOp))});
      }
    }
  }
  else if (assemblyLine->operatorLabel == "arrE")
  {
    if (array_table.find(assemblyLine->firstOp) != array_table.end())
    {
      array_table[assemblyLine->firstOp].push_back(this->getValue(assemblyLine->secondOp));
    }
    else
    {
      throw (int)ErrorVariableUnknown;
    }
  }
  else if (assemblyLine->operatorLabel == "push")
  {
    if (assemblyLine->firstOp == "RET_A")
    {
      prev_state.push(data_mem);
      prev_state_array.push(array_table);

      return_addresses.push(stoi(assemblyLine->secondOp));
    }
    else if (assemblyLine->firstOp == "PARAM")
    {
      params.push({ assemblyLine->secondOp, this->getValue(assemblyLine->secondOp) });
    }
    else if (assemblyLine->firstOp == "PARAM_ARR")
    {
      params_array.push({ assemblyLine->secondOp, array_table[assemblyLine->secondOp] });
      params_convert.insert({ assemblyLine->secondOp, "PARAM_ARR_" + params_convert_count++});
    }
  }
  else if (assemblyLine->operatorLabel == "pop")
  {
    if (assemblyLine->firstOp == "RET_A")
    {
        // if params is not empty then restore the prev_state
        return_addresses.pop();

        if (!prev_state.empty())
        {
            auto prev = prev_state.top();
            prev_state.pop();

            data_mem = prev;
        }

        if (!prev_state_array.empty())
        {
            auto prev = prev_state_array.top();
            prev_state_array.pop();
            bool isChanged = false;

            for (auto it = prev.begin(); it != prev.end(); it++)
            {
                if (params_convert.find(it->first) != params_convert.end())
                {
                    string key = params_convert[it->first];
                    prev[it->first] = array_table[params_convert[it->first]];
                    isChanged = true;
                }
            }

            array_table = isChanged ? prev : array_table;
        }
    }
  }
  else if (assemblyLine->operatorLabel == "and")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left && right);
    registers["CMPR"] = left && right;
  }
  else if (assemblyLine->operatorLabel == "or")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left || right);
    registers["CMPR"] = left || right;
  }
  else if (assemblyLine->operatorLabel == "b_and")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left & right);
  }
  else if (assemblyLine->operatorLabel == "b_or")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left | right);
  }
  else if (assemblyLine->operatorLabel == "xor")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left ^ right);
  }
  else if (assemblyLine->operatorLabel == "sh_left")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left << right);
  }
  else if (assemblyLine->operatorLabel == "sh_right")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left >> right);
  }
  else if (assemblyLine->operatorLabel == "add")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left + right);
  }
  else if (assemblyLine->operatorLabel == "sub")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left - right);
  }
  else if (assemblyLine->operatorLabel == "mod")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left % right);
  }
  else if (assemblyLine->operatorLabel == "div")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left / right);
  }
  else if (assemblyLine->operatorLabel == "multi")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    this->setValue(assemblyLine->firstOp, left * right);
  }
  else if (assemblyLine->operatorLabel == "not")
  {
    left = getValue(assemblyLine->firstOp);
    this->setValue(assemblyLine->firstOp, !left);
  }
  else if (assemblyLine->operatorLabel == "copy")
  {
    if (array_table.find(assemblyLine->firstOp) != array_table.end() &&
      array_table.find(assemblyLine->secondOp) != array_table.end()
    ) 
    {
      unsigned long minSize = min(array_table[assemblyLine->firstOp].size(), array_table[assemblyLine->secondOp].size());
      for (int i = 0; i < minSize; i++)
      {
        array_table[assemblyLine->firstOp][i] = array_table[assemblyLine->secondOp][i];
      }
    }
    else
    {
      throw (int)ErrorInvalidOpCode;
    }
  }
  else if (assemblyLine->operatorLabel == "len")
  {
    if (array_table.find(assemblyLine->secondOp) != array_table.end() &&
      (data_mem.find(assemblyLine->firstOp) != data_mem.end() || global_vars.find(assemblyLine->firstOp) != global_vars.end())
    )
    {
      this->setValue(assemblyLine->firstOp, array_table[assemblyLine->secondOp].size());
    }
    else
    {
      throw (int)ErrorInvalidOpCode;
    }
  }
  else if (assemblyLine->operatorLabel == "eqf")
  {
    if (array_table.find(assemblyLine->secondOp) != array_table.end() && array_table.find(assemblyLine->firstOp) != array_table.end())
    {
      registers["CMPR"] = array_table[assemblyLine->firstOp] == array_table[assemblyLine->secondOp];
    }
    else
    {
      throw (int)ErrorInvalidOpCode;
    }
  }
  else if (assemblyLine->operatorLabel == "scan")
  {
    string target;

    getline(cin, target);

    if (atoi(target.c_str()))
      this->setValue(assemblyLine->firstOp, stoi(target));
    else if (target.length() >= 1)
    {
      vector<int> param;

      for (int i = 0; i < target.size(); i++)
      {
        param.push_back((int)target[i]);
      }
      
      if (array_table.find(assemblyLine->firstOp) != array_table.end())
          array_table[assemblyLine->firstOp] = param;
      else
          this->setValue(assemblyLine->firstOp, param[0]);
    }
  }
  else if (assemblyLine->operatorLabel == "print")
  {
    if (!this->isArrayVariable(assemblyLine->firstOp) || assemblyLine->firstOp == "RET_V")
    {
      left = this->getValue(assemblyLine->firstOp);
    }

    left = this->getValue(assemblyLine->firstOp);
    right = this->getValue(assemblyLine->secondOp);
    
    char tmp = (char) left;
    
    switch (right)
    {
      case 0:
        cout << left << endl;
        break;
      case 1:
        cout << left;
        break;
      case 2:
        cout << tmp << endl;
        break;
      case 3:
        cout << tmp;
        break;
      case 4:
        cout << this->printArray(array_table[assemblyLine->firstOp]) << endl;
        break;
      case 5:
        cout << this->printArray(array_table[assemblyLine->firstOp]) << endl;
        break;
      case 6:
        cout << this->printString(array_table[assemblyLine->firstOp]);
        break;
      case 7:
        cout << this->printString(array_table[assemblyLine->firstOp]) << endl;
        break;
      default:
        throw (int)ErrorInvalidOpCode;
    }
  }
  else if (assemblyLine->operatorLabel == "rand")
  {
    int minBound = this->getValue(assemblyLine->firstOp);
    int maxBound = this->getValue(assemblyLine->secondOp);
    int value = (rand() % maxBound) + minBound;
    registers["CMPR"] = value;
  }
  else if (assemblyLine->operatorLabel == "app")
  {
    if (array_table.find(assemblyLine->secondOp) != array_table.end())
    {
        int value = this->getValue(assemblyLine->firstOp);
        array_table[assemblyLine->secondOp].push_back(value);
    }
  }
  else if (assemblyLine->operatorLabel == "rm")
  {
    if (array_table.find(assemblyLine->secondOp) != array_table.end())
    {
        vector<int> newArray;
        int value = this->getValue(assemblyLine->firstOp);

        for (int index = 0; index < array_table[assemblyLine->secondOp].size(); index++)
        {
            if (array_table[assemblyLine->secondOp][index] == value) continue;
            newArray.push_back(array_table[assemblyLine->secondOp][index]);
        }

        array_table[assemblyLine->secondOp] = newArray;
    }
  }
  else if (assemblyLine->operatorLabel == "eq")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    registers["CMPR"] = left == right;
  }
  else if (assemblyLine->operatorLabel == "neq")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    registers["CMPR"] = left != right;
  }
  else if (assemblyLine->operatorLabel == "lt")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    registers["CMPR"] = left < right;
  }
  else if (assemblyLine->operatorLabel == "gt")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    registers["CMPR"] = left > right;
  }
  else if (assemblyLine->operatorLabel == "lte")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    registers["CMPR"] = left <= right;
  }
  else if (assemblyLine->operatorLabel == "gte")
  {
    left = getValue(assemblyLine->firstOp);
    right = getValue(assemblyLine->secondOp);
    registers["CMPR"] = left >= right;
  }
  else if (assemblyLine->operatorLabel == "delete")
  {
    if (this->isArrayVariable(assemblyLine->firstOp))
    {
      array_table.erase(assemblyLine->firstOp);
    }
    else if (this->isDataMem(assemblyLine->firstOp))
    {
      data_mem.erase(assemblyLine->firstOp);
    }
    else
    {
      throw (int)ErrorVariableUnknown;
    }
  }
}

bool Processor::isLoadValid(string target)
{
  return (this->isRegister(target) ||
        this->isDataMem(target) ||
        this->isGlobal(target) ||
        this->isArrayInst(target));
}

bool Processor::isNumber(string target)
{
    if (target[0] == '-')
    {
        if (target.length() <= 1) return false;
        string oldTarget = target;
        target = "";
        for (int index = 1; index < oldTarget.length(); index++)
        {
            target += oldTarget[index];
        }
    }

    bool isNumber = true;

    for(string::const_iterator k = target.begin(); k != target.end(); ++k)
        isNumber = isdigit(*k) && isNumber;

    return isNumber;
}

bool Processor::isRegister(string target)
{
  for (auto i=registers.begin(); i != registers.end(); i++)
  {
    if (i->first == target)
    {
      return true;
    }
  }
  return false;
}

bool Processor::isDataMem(string target)
{
  for (auto i=data_mem.begin(); i != data_mem.end(); i++)
  {
    if (i->first == target)
    {
      return true;
    }
  }
  return false;
}

bool Processor::isGlobal(string target)
{
  for (auto i=global_vars.begin(); i != global_vars.end(); i++)
  {
    if (i->first == target)
    {
      return true;
    }
  }
  return false;
}

bool Processor::isArrayInst(string target)
{
  if (target.find('[') == string::npos || target.find(']') == string::npos) return false;
  string tmpTarg = target.substr(0, target.find("["));
  for (auto i=array_table.begin(); i != array_table.end(); i++)
  {
    if (i->first == tmpTarg)
    {
      return true;
    }
  }
  return false;
}

bool Processor::isArrayVariable(string target)
{
  return array_table.find(target) != array_table.end();
}

string Processor::printArray(vector<int> targ_array)
{
  string ans = "[";

  for (int i = 0; i < targ_array.size() - 1; i++)
  {
    ans += to_string(targ_array[i]) + ", ";
  }

  ans += to_string(targ_array[targ_array.size() - 1]);
  ans += "]";
  
  return ans;
}

string Processor::printString(vector<int> targ_string)
{
  string ans = "";

  for (auto i = targ_string.begin(); i != targ_string.end(); i++)
  {
    char inst = (char) *i;
    ans += inst;
  }

  return ans;
}

int Processor::getValue(string target)
{
  int ans = INT_MIN;

  if (this->isDataMem(target))
  {
    ans = data_mem[target];
  }
  else if (this->isRegister(target))
  {
      ans = registers[target];
  }
  else if (this->isNumber(target))
  {
    ans = stoi(target);
  }
  else if (this->isGlobal(target))
  {
    ans = global_vars[target];
  }
  else if (this->isArrayInst(target))
  {
    string array_label = target.substr(0, target.find("["));
    int index = this->getValue(target.substr(target.find("[") + 1, target.find("]") - target.find("[") - 1));

    string debugString = target.substr(target.find("[") + 1, target.find("]") - target.find("[") - 1);

    if (array_table[array_label].size() <= index|| index < 0) 
    { 
        index = 0;
    }
    
    ans = array_table[array_label][index];
  }
  else if (this->isArrayVariable(target))
  {
    // do nothing
  }
  else if (target == "PARAM")
  {
    ans = get<1>(params.front());
    params.pop();
  }
  else
  {
    throw (int)ErrorInvalidArithOp;
  }
  return ans;
}

void Processor::setValue(string target, unsigned long value)
{
  if (this->isArrayInst(target))
  {
    string array_label = target.substr(0, target.find("["));
    int index = this->getValue(target.substr(target.find("[") + 1, target.find("]") - target.find("[") - 1));
    if (array_table[array_label].size() <= index || index < 0) 
    { 
        return; 
    }
    array_table[array_label][index] = (int)value;
  }
  else if (this->isDataMem(target))
  {
    data_mem[target] = (int)value;
  }
  else if (this->isRegister(target))
  {
    registers[target] = (int)value;
  }
  else if (this->isGlobal(target))
  {
    global_vars[target] = (int)value;
  }
  else if (this->isNumber(target))
  {
    // do nothing
  }
  else
  {
    throw (int)ErrorInvalidArithOp;
  }
}

