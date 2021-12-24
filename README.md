# LiteLoader Database API

# Usage

## Session

> Header `<Session.h>`  
> Namespace `LLDB`

### `::newSession`

- Summary: Create a session  
- Params: 
  - dt: The type of the database which you want to connect
- Return: The `Session` object
- Note: The object is stored in heap. If you don't need them, plz call `Session::release` to release it.
- Example:
```cpp
auto sess = newSession(DatabaseType::SQLite);
```

### `Session::open`
- Summary: Open the database connection
- Params:
  - args: The `ConnectionArgs` object
- Return: `void`
- Throw: `LLDB::Exception`
- Example
```cpp
sess.open({})
```