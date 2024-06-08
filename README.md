# stdEx

Several ideas for extensions for standard template library

## stdEx::bitmap
Added ability to use all usual shift types: Arithmetic, Logical and Circular. Logical works the same as usual
 
 Arithmetic:
```cpp
stdEx::bitset<10> bst = 0b1110010110;
std::cout << bst << '\n'; // result: 1110010110
bst.SetShiftType(stdEx::ShiftTypes::Arithmetic);
bst >>= 1;
std::cout << bst << '\n'; // result: 1111001011
```

Circular:
```cpp
stdEx::bitset<10> bst = 0b0000000001;
std::cout << bst << '\n'; // result: 0000000001
bst.SetShiftType(stdEx::ShiftTypes::Circular);
bst >>= 1;
std::cout << bst << '\n'; // result: 1000000000
```
## stdEx::map
Allows to set and use default value when nonexistent one has been chosen
```cpp
stdEx::map<int, std::string> m;
b[0] = "zero";
b.SetDefaultValue("default");
std::cout << b[0] << '\n'; // result: zero
std::cout << b[134] << '\n'; // result: default
std::cout << b[13] << '\n'; // result: default
```

## stdEx::multimap and stdEx::unordered_multimap 
Allows to get vector of all corresponding values by key using [] operator from your multimap

Result:
```cpp
stdEx::multimap<int, std::string> a;
a.insert({ 1, "one" });
a.insert({ 1, "uno" });
std::vector<std::string> q = a[1];
for(const auto& i : q)
{
    std::cout << i << '\n';
}
/*
    result: 
    one
    uno
*/
```
