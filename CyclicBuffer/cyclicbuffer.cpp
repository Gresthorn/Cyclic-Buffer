#include "cyclicbuffer.h"

CyclicBuffer::CyclicBuffer(unsigned int buf_size, int *success)
{
    buffer_error_code = BUFFER_UNDEFINED_ERROR;

    // check if buf_size is reasonable number
    if(buf_size==0)
    {
        (*success) = buffer_error_code = BUFFER_INVALID_SIZE;
        return;
    }

    buffer = new unsigned char[buf_size];
    // try to allocate requested memory
    if(buffer==NULL)
    {
        (*success) = buffer_error_code = BUFFER_ALLOCATION_ERROR;
        return;
    }

    // set the bottom and top indexes to default values
    bottom_index = 0;
    top_index = buf_size-1;

    // set the initial buffer size
    buffer_size = buf_size;

    // prepare buffer
    ClearBuffer();

    write_ptr = read_ptr = 0;
}

void CyclicBuffer::Push(unsigned char ch)
{
    buffer[write_ptr] = ch;
    if(++write_ptr > top_index)
        write_ptr = bottom_index;
}

unsigned char CyclicBuffer::Pop()
{
    // Since write_ptr points to first space to be overwritten in next push call,
    // this position does not contain fresh data. There is nothing to read.
    if(read_ptr==write_ptr)
        return (unsigned char)NULL;

    if(read_ptr == top_index)
    {
        read_ptr = bottom_index;
        return buffer[top_index];
    }

    return buffer[read_ptr++];
}

CyclicBuffer::buffer_error CyclicBuffer::SetPopIndex(unsigned int index)
{
    // check if index is not outside the range
    if(index > top_index)
    {
        buffer_error_code = BUFFER_INDEX_GREATER;
        return buffer_error_code;
    }
    else if(index < bottom_index)
    {
        buffer_error_code = BUFFER_INDEX_LESS;
        return buffer_error_code;
    }

    read_ptr = index;
    buffer_error_code = BUFFER_OK;
    return buffer_error_code;
}

CyclicBuffer::buffer_error CyclicBuffer::SetPushIndex(unsigned int index)
{
    // check if index is not outside the range
    if(index > top_index)
    {
        buffer_error_code = BUFFER_INDEX_GREATER;
        return buffer_error_code;
    }
    else if(index < bottom_index)
    {
        buffer_error_code = BUFFER_INDEX_LESS;
        return buffer_error_code;
    }

    write_ptr = index;
    buffer_error_code = BUFFER_OK;
    return buffer_error_code;
}

CyclicBuffer::buffer_error CyclicBuffer::SetTopIndex(unsigned int index)
{
    // check if new index is not more than allocated array
    if(index > (buffer_size-1))
    {
        buffer_error_code = BUFFER_INDEX_GREATER;
        return buffer_error_code;
    }

    // check if our new index is not less than our current bottom index
    if(index < bottom_index)
    {
        buffer_error_code = BUFFER_INDEX_COLLISION_LESS;
        return buffer_error_code;
    }

    // clear bytes that are to be added
    if(top_index < index)
    {
        for(unsigned int i = (top_index+1); i<=index; i++)
            buffer[i] = (unsigned char)NULL;
    }

    // now everything should be OK to set
    top_index = index;

    // make correction of other pointers
    if(write_ptr > top_index)
        write_ptr = bottom_index; // so the "new" last byte will not be rewritten twice
    if(read_ptr > top_index)
        read_ptr = bottom_index; // so the "new" last byte will not be read twice

    buffer_error_code = BUFFER_OK;
    return buffer_error_code;
}

CyclicBuffer::buffer_error CyclicBuffer::SetBottomIndex(unsigned int index)
{
    // check if new index is not more than allocated array
    if(index > (buffer_size-1))
    {
        buffer_error_code = BUFFER_INDEX_GREATER;
        return buffer_error_code;
    }

    // check if our new index is not higher than our current top index
    if(index > top_index)
    {
        buffer_error_code = BUFFER_INDEX_COLLISION_GREATER;
        return buffer_error_code;
    }

    // clear bytes that are to be added
    if(index < bottom_index)
    {
        for(unsigned int i = index; i<=(bottom_index-1); i++)
            buffer[i] = (unsigned char)NULL;
    }

    // now everything should be OK to set
    bottom_index = index;

    // make correction of other pointers
    if(write_ptr < bottom_index)
        write_ptr = bottom_index;
    if(read_ptr < bottom_index)
        read_ptr = bottom_index;

    buffer_error_code = BUFFER_OK;
    return buffer_error_code;
}

CyclicBuffer::buffer_error CyclicBuffer::ReallocBuffer(unsigned int size)
{
    // check if size is natural number
    if(!size)
    {
        buffer_error_code = BUFFER_INCORRECT_SIZE;
        return buffer_error_code;
    }

    // if size is equal to current size, no special operations are needed
    if(size==buffer_size)
    {
        buffer_error_code = BUFFER_OK;
        return buffer_error_code;
    }

    // try to allocate new block of memory for buffer
    unsigned char * temp_buf_ptr = (unsigned char * )realloc(buffer, sizeof(unsigned char)*size);

    if(temp_buf_ptr==NULL)
    {
        // allocation of new block of data failed
        buffer_error_code = BUFFER_ALLOCATION_ERROR;
        return buffer_error_code;
    }

    // now the allocation was successful
    buffer = temp_buf_ptr;
    buffer_size = size;

    // make correction on pointers/indexes
    unsigned int highest_index = (buffer_size-1);

    if(top_index > highest_index)
        top_index = highest_index;

    if(bottom_index > highest_index)
        bottom_index = highest_index;

    if(read_ptr > highest_index)
        read_ptr = bottom_index;

    if(write_ptr > highest_index)
        write_ptr = 0;

    buffer_error_code = BUFFER_OK;
    return buffer_error_code;
}

void CyclicBuffer::ResetBuffer()
{
    top_index = buffer_size-1;
    read_ptr = write_ptr = bottom_index = 0;

    for(unsigned int i = 0; i <= top_index; i++)
        buffer[i] = (unsigned char)NULL;
}

void CyclicBuffer::ClearBuffer()
{
    for(unsigned int i=bottom_index; i<top_index; i++)
    {
        buffer[i] = (unsigned char)NULL;
    }
}

unsigned char CyclicBuffer::GetValueAt(unsigned int index, bool use_offset, bool look_outside_borders)
{
    unsigned int index_t = index;

    // make correction of index if needed
    if(use_offset)
        index_t += bottom_index;

    // index out of range
    if(index_t >= buffer_size)
        return (unsigned char)NULL;

    // if searching outside of borders is not allowed
    if(!look_outside_borders && (index_t>top_index || index_t<bottom_index))
        return (unsigned char)NULL;

    return buffer[index_t];
}

CyclicBuffer::buffer_error CyclicBuffer::SetValueAt(unsigned int index, unsigned char value, bool use_offset, bool look_outside_borders)
{
    unsigned int index_t = index;

    // make correction of index if needed
    if(use_offset)
        index_t += bottom_index;

    // index out of range
    if(index_t >= buffer_size)
        return BUFFER_INDEX_OUT_OF_RANGE;

    // if searching outside of borders is not allowed
    if(!look_outside_borders && (index_t>top_index || index_t<bottom_index))
        return BUFFER_INDEX_OUT_OF_RANGE;

    buffer[index_t] = value;

    return BUFFER_OK;
}
