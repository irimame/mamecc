#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./mamecc "$input" > tmp.s
  cc -z noexecstack -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0    'main() { 0; }'
assert 42   'main() { 42; }'
assert 21   'main() { 5+20-4; }'
assert 41   "main() {  12 + 34 - 5 ; }"
assert 47   'main() { 5 +6* 7; }'
assert 15   'main() { 5*(9 -6); }'
assert 4    'main() { ( 3 + 5 )/ 2; }'
assert 2    'main() {  - 3 + 5; }'
assert 15   'main() { -3*-5; }'
assert 9    'main() { +3 * ( + 6  /2); }'
assert 1    'main() { +3 * ( + 6  /2) < 10; }'
assert 0    'main() { 48 <= 5 ++6* 7; }'
assert 0    'main() { -3*-5 > 15; }'
assert 1    'main() { -3*-5 >= 15; }'
assert 1    'main() { 2--6 == 50 +-6* 7; }'
assert 0    'main() {  9 !=  +3 * ( + 6  /2) ; }'
assert 3    'main() { a = 3; b = 5 * 6 - 8; a; }'
assert 3    'main() { a = 3; b = 5 * 6 - 8; a + b / 2; a; }'
assert 14   'main() { a = 3; b = 5 * 6 - 8; a + b / 2; }'
assert 6    'main() { foo = 1; bar = 2 + 3; foo + bar; }'
assert 6    'main() {  x = 3; foo = 2 * x; foo; }'
assert 14   'main() { a = 3; b = 5 * 6 - 8; return a + b / 2; }'
assert 14   'main() { a = 3; b = 5 * 6 - 8; return (a + b / 2); }'
assert 0    'main() { i = 0; a = 3; if (a < 3) i = i + 1; return i; }'
assert 2    'main() { i = 1; a = 2; if (a < 3) i = i + 1; return i; }'
assert 2    'main() { i = 0; a = 3; if (a < 3) i = i + 1; else i = i + 2; return i; }'
assert 1    'main() { i = 0; a = 2; if (a < 3) i = i + 1; else i = i + 2; return i; }'
assert 3    'main() { i = 0; while (i < 3) i = i + 1; return i; }'
assert 4    'main() { a = 1; for(i=0; i < 3; i= i+1) a = a * 2; return a / 2; }'
assert 6    'main() { foo = 1; bar = 2 + 3; foo + bar; }'
assert 4    'main() { a = 1; for(i=0; i < 3; i= i+1) a = a * 2; return a / 2;}'
assert 2    'main() { i = 0; a = 3; if (a < 3) {a = a + 2; a = a * 2; a = a - 2; a = a - 2;} else {i = i + 4; i = i / 2;} i; }'
assert 3    'main() { i = 0; a = 3; if (a < 3) {a = a + 2; a = a * 2; a = a - 2; a = a - 2;} else {i = i + 4; i = i / 2;} a; }'
assert 0    'main() { i = 0; a = 2; if (a < 3) {a = a + 2; a = a * 2; a = a - 2; a = a - 2;} else {i = i + 4; i = i / 2;} return i; }'
assert 4    'main() { i = 0; a = 2; if (a < 3) {a = a + 2; a = a * 2; a = a - 2; a = a - 2;} else {i = i + 4; i = i / 2;} return a; }'
assert 1    'main() { return ret_one(); } ret_one() { return 1; }'
assert 3    'ret_two() { a = 1; b = 1; return a + b; } main() { return ret_two() + 1; }'
assert 3    'ret_three() { a = 1; b = 2; a + b; } main() { c = ret_three(); return c; }'
assert 6    'main() { return (ret_one() + ret_two() + ret_three()); } ret_one() { a = 1; return a; } ret_two() { a = 1; b = 1; return a + b; } ret_three() { a = ret_one(); b = ret_two(); return a + b; }'
assert 42   'main() { return ret_num(42); } ret_num(n) { return n; }'
assert 42   'main() { a = 21; return add_21(a); } add_21(n) { b = 21; return (n + b); }'
assert 10   'main() { return add(6, 4); } add(a, b) { return (a + b); }'
assert 120  'main() { n = factorial(5); return n; } factorial(n) { if (n == 1) return 1; return (n * factorial(n - 1)); }'
assert 1    'main() { return fibonacci(1); } fibonacci(n) { if (n == 1) return 1; if (n == 2) return 1; return (fibonacci(n - 1) + fibonacci(n - 2)); }' 
assert 1    'main() { return fibonacci(2); } fibonacci(n) { if (n == 1) return 1; if (n == 2) return 1; return (fibonacci(n - 1) + fibonacci(n - 2)); }' 
assert 2    'main() { return fibonacci(3); } fibonacci(n) { if (n == 1) return 1; if (n == 2) return 1; return (fibonacci(n - 1) + fibonacci(n - 2)); }' 
assert 3    'main() { return fibonacci(4); } fibonacci(n) { if (n == 1) return 1; if (n == 2) return 1; return (fibonacci(n - 1) + fibonacci(n - 2)); }' 
assert 5    'main() { return fibonacci(5); } fibonacci(n) { if (n == 1) return 1; if (n == 2) return 1; return (fibonacci(n - 1) + fibonacci(n - 2)); }' 
assert 5    'main() { a = 2; b = 3; return fibonacci(a + b); } fibonacci(n) { if (n == 1) return 1; if (n == 2) return 1; return (fibonacci(n - 1) + fibonacci(n - 2)); } ' 
assert 18   'main() { a = 6; b = 4; return add4(add4(a) + b); } add4(a) { return (a + 4); }'
assert 10   'main() { a = 6; b = 4; return add(a, b); } add(a, b) { return (a + b); }'
assert 5    'main() { a = 2; b = 3; C = add(a, b); return fibonacci(C); } fibonacci(n) { if (n == 1) return 1; if (n == 2) return 1; return (fibonacci(n - 1) + fibonacci(n - 2)); } add (a, b) { return (a + b); }' 
assert 5    'main() { a = 2; b = 3; return fibonacci(add(a, b)); } fibonacci(n) { if (n == 1) return 1; if (n == 2) return 1; return (fibonacci(n - 1) + fibonacci(n - 2)); } add (a, b) { return (a + b); }' 

echo OK