#pragma once
#include <glad/glad.h>
class indexBuffer{
  private:
  GLuint renderID;
  public:
  indexBuffer(const void* data,GLuint count);
  ~indexBuffer();
  void bind()const;
  void unbind()const;
};
