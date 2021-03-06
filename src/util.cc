#include <string.h>

#include <node.h>
#include <v8.h>

using namespace v8;

Local<Value> createNativeArray(Handle<String> name, size_t length) {
  HandleScope scope;

  Handle<Value>    fun_val  = Context::GetCurrent()->Global()->Get(name);
  Handle<Function> fun      = Handle<Function>::Cast(fun_val);

  const unsigned argc = 1;
  Local<Value> argv[argc] = { Local<Value>::New(Uint32::New(length)) };

  Local<Object> result = fun->NewInstance(argc, argv);

  return scope.Close(result);
}

Local<Value> newInt8Array(size_t length) {
  return createNativeArray(String::New("Int8Array"), length);
}

Local<Value> newInt16Array(size_t length) {
  return createNativeArray(String::New("Int16Array"), length);
}

Local<Value> newInt32Array(size_t length) {
  return createNativeArray(String::New("Int32Array"), length);
}

Handle<Value> fillArray(Handle<Value> destination, void* source, size_t length) {
  HandleScope scope;

#ifdef TYPED_ARRAY_TYPE_PRE_0_11
  Handle<Object> dest = Handle<Object>::Cast(destination);
  Handle<Object> buffer = dest->Get(String::New("buffer"))->ToObject();
  unsigned int offset       = dest->Get(String::New("byteOffset"))->Uint32Value();
  unsigned int bufferLength = dest->Get(String::New("byteLength"))->Uint32Value();
  char* baseAddress = &((char*) buffer->GetIndexedPropertiesExternalArrayData())[offset];
#else
  Handle<TypedArray> dest = Handle<TypedArray>::Cast(destination);
  unsigned int offset = dest->ByteOffset();
  unsigned int bufferLength = dest->ByteLength();
  void* baseAddress = dest->BaseAddress();
#endif

  if (offset + length > bufferLength) {
    return ThrowException(Exception::TypeError(String::New("No space for data.")));
  }

  memcpy(baseAddress, source, length);

  return scope.Close(Undefined());
}
