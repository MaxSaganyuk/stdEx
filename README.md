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
m[0] = "zero";
m.SetDefaultValue("default");
std::cout << m[0] << '\n'; // result: zero
std::cout << m[134] << '\n'; // result: default
std::cout << m[13] << '\n'; // result: default
```

## stdEx::multimap and stdEx::unordered_multimap 
Allows to get vector of all corresponding values by key using [] operator from your multimap

Result:
```cpp
	std::string q = "2";
	stdEx::ValWithBackup<std::string> a(q);
	q = "3";
	std::cout << a.Get() << '\n';
	a.Set("5");
	std::cout << a.Get() << '\n';
```

## stdEx::ValWithBackup
A template class which holds data and backup reference. Allows to make sure that specific reference to some value is used untill we have set actual data. Useful in situations where we have to have a general default value until we set a concrete one

	std::string q = "2"; // default data
	stdEx::ValWithBackup<std::string> a(q); 
	q = "3"; // default data has been changed
	std::cout << a.Get() << '\n'; // yields 3, since it refs "default" data address
	a.Set("5"); // now we have concrete data, our own
	std::cout << a.Get() << '\n'; // yields 5
*/
```