# **SynchrotronComponent** Test results

## Functional results

- ### Using `std::set`

| Functional test | Expected | MinGW 4.9.2 x86 | MSVC 2013 x86 |
| --- | :---: | :---: | :---: |
| Kept order when adding with std::init_list	| true | **true** | false    |
| Kept order when adding with addOutput()	| true | **true** | false    |
| Won't allow duplicates			| true | **true** | **true** |

| Performance Test (10 x 10,000 items) | MinGW Average (ms) | MSVC Average (ms) |
| --- | :---: | :---: |
| addOutput()                          |       9 |       2 |
| emit()                               |       0 |       0 |
| removeOutput()                       |       9 |       1 |
| Total time taken                     |     200 |      61 |
| Total size of signalprovider (bytes) | 600,060 | 280,028 |

- ### Using `std::list`

| Parameter | Expected | MinGW 4.9.2 x86 | MSVC 2013 x86 |
| --- | :---: | :---: | :---: |
| Kept order when adding with std::init_list	| true | **true** | **true** |
| Kept order when adding with addOutput()	| true | **true** | **true** |
| Won't allow duplicates			| true | false    | false    |

| Performance Test (10 x 10,000 items) | MinGW Average (ms) | MSVC Average (ms) |
| --- | :---: | :---: |
| addOutput()                          |       2 |       1 |
| emit()                               |       0 |       0 |
| removeOutput()                       |     130 |     114 |
| Total time taken                     |    1406 |    1181 |
| Total size of signalprovider (bytes) | 280,028 | 280,028 |

- ### Using `std::forward_list`

| Parameter | Expected | MinGW 4.9.2 x86 | MSVC 2013 x86 |
| --- | :---: | :---: | :---: |
| Kept order when adding with std::init_list	| true | false | false |
| Kept order when adding with addOutput()	| true | false | false |
| Won't allow duplicates			| true | false | false |

| Performance Test (10 x 10,000 items) | MinGW Average (ms) | MSVC Average (ms) |
| --- | :---: | :---: |
| addOutput()                          |       8 |       1 |
| emit()                               |       0 |       0 |
| removeOutput()                       |     103 |      88 |
| Total time taken                     |    1125 |     917 |
| Total size of signalprovider (bytes) | 200,020 | 200,020 |

- ### Using `std::vector`

| Parameter | Expected | MinGW 4.9.2 x86 | MSVC 2013 x86 |
| --- | :---: | :---: | :---: |
| Kept order when adding with std::init_list	| true | **true** | **true** |
| Kept order when adding with addOutput()	| true | **true** | **true** |
| Won't allow duplicates			| true | false    | false    |

| Performance Test (10 x 10,000 items) | MinGW Average (ms) | MSVC Average (ms) |
| --- | :---: | :---: |
| addOutput()                          |       8 |       1 |
| emit()                               |       0 |       0 |
| removeOutput()                       |      32 |      29 |
| Total time taken                     |     417 |     322 |
| Total size of signalprovider (bytes) | 360,036 | 360,036 |

- ### Using `std::set` with `.insert( .end() , _val)`

| Parameter | Expected | MinGW 4.9.2 x86 | MSVC 2013 x86 |
| --- | :---: | :---: | :---: |
| Kept order when adding with std::init_list	| true | **true** | false    |
| Kept order when adding with addOutput()	| true | **true** | false    |
| Won't allow duplicates			| true | **true** | **true** |

| Performance Test (10 x 10,000 items) | MinGW Average (ms) | MSVC Average (ms) |
| --- | :---: | :---: |
| addOutput()                          |       9 |       2 |
| emit()                               |       0 |       0 |
| removeOutput()                       |       9 |       1 |
| Total time taken                     |     192 |      54 |
| Total size of signalprovider (bytes) | 600,060 | 280,028 |

- ### Using `std::set` with custom comparer

| Parameter | Expected | MinGW 4.9.2 x86 | MSVC 2013 x86 |
| --- | :---: | :---: | :---: |
| Kept order when adding with std::init_list	| true | **true** | false    |
| Kept order when adding with addOutput()	| true | **true** | false    |
| Won't allow duplicates			| true | **true** | **true** |

| Performance Test (10 x 10,000 items) | MinGW Average (ms) | MSVC Average (ms) |
| --- | :---: | :---: |
| addOutput()                          |      10 |       2 |
| emit()                               |       0 |       0 |
| removeOutput()                       |       9 |       2 |
| Total time taken                     |     201 |      60 |
| Total size of signalprovider (bytes) | 600,060 | 280,028 |
