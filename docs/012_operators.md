# Batch Script - Operators

---



---

An operator is a symbol that tells the compiler to perform specific mathematical or logical manipulations.

In batch script, the following types of operators are possible.

* Arithmetic operators
* Relational operators
* Logical operators
* Assignment operators
* Bitwise operators

## Arithmetic Operators

Batch script language supports the normal Arithmetic operators as any language. Following are the Arithmetic operators available.

[Show Example](/batch_script/batch_script_arithmetic_operators.htm)

| Operator | Description | Example |
| --- | --- | --- |
| &plus; | Addition of two operands | 1 &plus; 2 will give 3 |
|  | Subtracts second operand from the first | 2 1 will give 1 |
| \* | Multiplication of both operands | 2 \* 2 will give 4 |
| / | Division of the numerator by the denominator | 3 / 2 will give 1.5 |
| % | Modulus operator and remainder of after an integer/float division | 3 % 2 will give 1 |

## Relational Operators

Relational operators allow of the comparison of objects. Below are the relational operators available.

[Show Example](/batch_script/batch_script_relational_operators.htm)

| Operator | Description | Example |
| --- | --- | --- |
| EQU | Tests the equality between two objects | 2 EQU 2 will give true |
| NEQ | Tests the difference between two objects | 3 NEQ 2 will give true |
| LSS | Checks to see if the left object is less than the right operand | 2 LSS 3 will give true |
| LEQ | Checks to see if the left object is less than or equal to the right operand | 2 LEQ 3 will give true |
| GTR | Checks to see if the left object is greater than the right operand | 3 GTR 2 will give true |
| GEQ | Checks to see if the left object is greater than or equal to the right operand | 3 GEQ 2 will give true |

## Logical Operators

Logical operators are used to evaluate Boolean expressions. Following are the logical operators available.

The batch language is equipped with a full set of Boolean logic operators like AND, OR, XOR, but only for binary numbers. Neither are there any values for TRUE or FALSE. The only logical operator available for conditions is the NOT operator.

[Show Example](/batch_script/batch_script_logical_operators.htm)

| Operator | Description |
| --- | --- |
| AND | This is the logical and operator |
| OR | This is the logical or operator |
| NOT | This is the logical not operator |

## Assignment Operators

Batch Script language also provides assignment operators. Following are the assignment operators available.

[Show Example](/batch_script/batch_script_assignment_operators.htm)

| Operator | Description | Example |
| --- | --- | --- |
| &plus;= | This adds right operand to the left operand and assigns the result to left operand | Set /A a = 5  a &plus;= 3  Output will be 8 |
| -= | This subtracts the right operand from the left operand and assigns the result to the left operand | Set /A a = 5  a -= 3  Output will be 2 |
| \*= | This multiplies the right operand with the left operand and assigns the result to the left operand | Set /A a = 5  a \*= 3  Output will be 15 |
| /= | This divides the left operand with the right operand and assigns the result to the left operand | Set /A a = 6  a/ = 3  Output will be 2 |
| %= | This takes modulus using two operands and assigns the result to the left operand | Set /A a = 5  a% = 3  Output will be 2 |

## Bitwise Operators

Bitwise operators are also possible in batch script. Following are the operators available.

[Show Example](/batch_script/batch_script_bitwise_operators.htm)

| Operator | Description |
| --- | --- |
| & | This is the bitwise and operator |
| | | This is the bitwise or operator |
| ^ | This is the bitwise xor or Exclusive or operator |

Following is the truth table showcasing these operators.

| p | q | p & q | p | q | p ^ q |
| --- | --- | --- | --- | --- |
| 0 | 0 | 0 | 0 | 0 |
| 0 | 1 | 0 | 1 | 1 |
| 1 | 1 | 1 | 1 | 0 |
| 1 | 0 | 0 | 1 | 1 |

