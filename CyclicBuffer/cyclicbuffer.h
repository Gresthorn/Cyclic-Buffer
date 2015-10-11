/*
 * Author:      Peter Mikula
 * Date:        10. 10. 2015
 * Version:     0.1.1
 *
 */

//! Cyclic buffer class for simplification of data management coming from
//! UWB sensor network via serial link (RS232, FTDI, ...).
/*!
  All data coming from UWB sensor network are stored in packets in specific
  format. Packets contain radar ID, time of measurement, packet number,
  coordinates of visible targets, CRC and ending character. Buffering data
  is important since data processing is not always in synchronization with
  data recieving. The following class manages memory and pointers for reading
  and writing processes. When the buffer is filled, writing begins from the zero
  index.
  */

#ifndef CYCLICBUFFER_H
#define CYCLICBUFFER_H

#include <cstdlib>
#include <cstddef>

class CyclicBuffer
{

public:

    //! Enumeration for error codes availible in buffer.
    /*!
      This enumeration type packages all possible error codes/states to be reported
      by the 'CyclicBuffer'. The latest code is always stored inside the 'buffer_error_code'
      variable.
     */
    enum buffer_error {
        BUFFER_OK = 0, /*!< Value is returned every time if the function ends properly. */
        BUFFER_INVALID_SIZE = 1, /*!< This value is present if user typed invalid buffer size as parameter. */
        BUFFER_ALLOCATION_ERROR = 2, /*!< If program was not able to allocate the memory for buffer. */
        BUFFER_INDEX_GREATER = 3, /*!< If user specify index that is greater than maximal (top_index). */
        BUFFER_INDEX_LESS = 4, /*!< If user specify index that is less than minimal (bottom_index). */
        BUFFER_INDEX_COLLISION_LESS = 5, /*!< If user specify new top index smaller than current bottom index */
        BUFFER_INDEX_COLLISION_GREATER = 6, /*! If user specify new bottom index greater than current top index */
        BUFFER_INCORRECT_SIZE = 7, /*!< Occures when user gives unallowed or 0 buffer size in reallocation function */
        BUFFER_INDEX_OUT_OF_RANGE = 8, /*!< Used when program request to set new value on index that is not present in range of indexes of bordered area or buffer memory block. */
        BUFFER_UNDEFINED_ERROR = 999 /*!< If this value is present, program catched the error, but could not identify its source (also initial error code set up in constructor). */
    };

    //! Construcor of buffer class. Ensures basic initialization and memory allocation.
    /*!
     * \brief The constructor will initialize all internal pointers to zero and
     * allocates required memory for data storage.
     * \param buffer_size an integer number representing the buffer size in bytes.
     * \param success if memory was allocated successfully, the return value is 0, otherwise the error code.
     */
    CyclicBuffer(unsigned int buf_size, int *success);

    //! Function is about to push new value to the buffer.
    /*!
     * \brief The function will write new character to the buffer while incrementing pointer from
     * the lastly written character to the new empty place.
     * \param ch New value to be written into buffer.
     */
    void Push(unsigned char ch);

    //! Function is about to retrieve the value from the buffer.
    /*!
     * \brief This function will retrieve the first byte that has not been read yet.
     * After the operation is done, it will increment the 'read_ptr'.
     * \return The unsigned char value from buffer.
     */
    unsigned char Pop(void);

    //! Function will set new index for pop function.
    /*!
     * \brief This function will set new 'read_ptr' value so in next pop function call
     * byte at this index will be returned. Function returns NULL character if there
     * are no new data to read yet.
     * \param index new index/value for 'read_ptr'.
     * \return If invalid index is set (greater than 'top_index' or less than 'bottom_index'), error code is returned, otherwise BUFFER_OK is returned.
     */
    buffer_error SetPopIndex(unsigned int index);

    //! Function will set new index for push function.
    /*!
     * \brief This function will set new 'write_ptr' value so in next push function call
     * byte at this index will be rewritten.
     * \param index new index/value for 'write_ptr'
     * \return If invalid index is set (greater than 'top_index' or less than 'bottom_index'), error code is returned, otherwise BUFFER_OK is returned.
     */
    buffer_error SetPushIndex(unsigned int index);

    //! Function allows to shift the top index of buffer.
    /*!
     * \brief Since the memory may be required to be smaller or greater for the buffer
     * (depends on application specific reasons) it is better to shift borders than allocating
     * all new array and copying values. This function will move the top border of buffer
     * to desired position. Note that if new memory is added by shifting, new bytes will be
     * automatically zeroed. If some bytes are cut off, cut bytes are remaining untouched.
     * Function will automatically modify 'read_ptr' and 'write_ptr' if they stay outside the range.
     * \param index new index to be set into 'top_index'.
     * \return If function correctly shifts the index, return value is BUFFER_OK, otherwise will return error code.
     */
    buffer_error SetTopIndex(unsigned int index);

    //! Function allows to shift the bottom index of buffer.
    /*!
     * \brief Since the memory may be required to be smaller or greater for the buffer
     * (depends on application specific reasons) it is better to shift borders than allocating
     * all new array and copying values. This function will move the bottom border of buffer
     * to desired position. Note that if new memory is added by shifting, new bytes will be
     * automatically zeroed. If some bytes are cut off, cut bytes are remaining untouched.
     * Function will automatically modify 'read_ptr' and 'write_ptr' if they stay outside the range.
     * \param index new index to be set into 'bottom_index'.
     * \return If function correctly shifts the index, return value is BUFFER_OK, otherwise will return error code.
     */
    buffer_error SetBottomIndex(unsigned int index);

    //! Function will resize total memory block availible to buffer.
    /*!
     * \brief This realloc function ensures that the currently used memory block availible to buffer
     * will be resized to desired size. Top and bottom indexes as well as read and write pointers
     * will be updated automatically. If size is less than top or bottom indexes, these are set
     * to maximal availible index in new memory block. If size is less than read or write pointer
     * indexes, those will be automatically set back to smallest index possible. Top and bottom borders
     * are updated firstly.
     * \param size contains new desired size for buffer memory block
     * \return The return value is error code if allocation problems occures or BUFFER_OK otherwise.
     */
    buffer_error ReallocBuffer(unsigned int size);

    //! Function ensures that all pointers and indexes will fit into current memory block.
    /*!
     * \brief This simple function should be called whenever a clean up of pointers and
     * indexes is needed (e.g. after 'ReallocBuffer' function). It will set maximal/minimal
     * indexes (borders of memory block), read and write pointers to fit currently availible
     * memory block. Function will also put all bytes to zero.
     */
    void ResetBuffer(void);

    //! Function will clear the buffer.
    /*!
     * \brief This function will replace all availible values in buffer with NULL
     * character. Note that only indexes from 'bottom_index' to 'top_index' are cleared.
     */
    void ClearBuffer(void);

    //! Function will return value at index specified.
    /*!
     * \brief If random access to the buffer array is required, this function can be used
     * to retrieve any byte from the buffer. If user request for value
     * that is outside of buffer borders, function will return NULL. However, if second parameter
     * is true, function will look also at indexes present in buffer memory block even if they
     * are outside the buffer borders.
     * \param index Position of byte in buffer array to be returned.
     * \param use_offset If value is true (default) index value of 0 will be equal to 'bottom_index'.
     * \param look_outside_borders If value is true, function will look also outside the buffer borders if index is in memory block availible. Default value is false.
     * \return Value on given index returned as unsigned char. NULL is returned, if index is out of range.
     */
    unsigned char GetValueAt(unsigned int index, bool use_offset = true, bool look_outside_borders = false);

    //! Function will set byte at desired index.
    /*!
     * \brief If random access to the buffer array is required, this function can be used
     * to set up any byte in the buffer. If new value is requested to be set on index outside
     * the range, function will return error code. It is possible to work inside the borders as
     * well as outside of bordered area if index value still points into memory block managed by buffer.
     * \param index Target position for new value.
     * \param value New value to be set.
     * \param use_offset If value is true (default) index value of 0 will be equal to 'bottom_index'.
     * \param look_outside_borders If value is true, function will look also outside the buffer borders if index is in memory block availible. Default value is false.
     * \return The return value is error code if index is out of range, or BUFFER_OK on success.
     */
    buffer_error SetValueAt(unsigned int index, unsigned char value, bool use_offset = true, bool look_outside_borders = false);

    //! Function returns the availible memory for buffer.
    /*!
     * \brief Function returns the total bytes availible in memory for the buffer.
     * This memory represents the maximum bytes that can be used. However, real buffer
     * size currently being used by the program can be less.
     * \return buffer size represented in bytes.
     */
    unsigned int GetTotalBufferSize(void) { return buffer_size; }

    //! Function returns the real buffer size.
    /*!
     * \brief Function returns the real buffer size since total amount of availible
     * memory can be stripped so not all bytes are being used by program. This method
     * is useful when it is required to resize buffer from time to time without need for
     * allocating new space.
     * \return the real buffer size in bytes.
     */
    unsigned int GetBufferSize(void) { return (top_index-bottom_index)+1; }

    //! Function returns the read pointer index.
    /*!
     * \brief In some situations program may need to know what is the current read
     * pointer position (so it can move it away and return there later). This function
     * will return the last unread byte position in the array.
     * \return Return value is the index of unread byte.
     */
    unsigned int GetPopIndex(void) { return read_ptr; }


    //! Function returns the write pointer index.
    /*!
     * \brief If it is required program may request the position of first byte that
     * is going to be replaced in next push call.
     * \return Index of byte to be rewritten by push call.
     */
    unsigned int GetPushIndex(void) { return write_ptr; }

    //! Function will return the smallest index of buffer memory array currently in use.
    /*!
     * \brief Buffer can be set up so not all availible allocated memory is being used.
     * Borders of buffer in its memory can be shifted, therefore this function allow
     * to check where the bottom border (index of buffer array) is positioned.
     * \return Unsigned integer representing the index of buffer array.
     */
    unsigned int GetBottomIndex(void) { return bottom_index; }

    //! Function will return the highest index of buffer memory array currently in use.
    /*!
     * \brief Buffer can be set up so not all availible allocated memory is being used.
     * Borders of buffer in its memory can be shifted, therefore this function allow
     * to check where the top border (index of buffer array) is positioned.
     * \return Unsigned integer representing the index of buffer array.
     */
    unsigned int GetTopIndex(void) { return top_index; }

    //! Function is the same as 'Push' function.
    void operator<<(unsigned char ch) { Push(ch); }

    //! Function does the same thing as 'Pop' function.
    void operator>>(unsigned char & ch) { ch = Pop(); }

private:

    //! Variable which stores the latest error code availible.
    /*!
      This variable is storing the latest error code catched by some of the
      function availible in the 'CyclicBuffer'. If the function does not return
      BUFFER_OK value, higher functions can still look at this variable to obtain
      the latest error code.
     */
    buffer_error buffer_error_code;

    //! Pointer to the buffer.
    /*!
      This is the pointer to the buffer itself.
     */
    unsigned char * buffer;

    //! Variable for total amount of bytes availible.
    /*!
      This variable stores the total number of bytes currently availible in memory
      for this buffer. Note that buffer can be resized, therefore this variable
      should be updated. Sometimes only the part of memory availible can serve
      as buffer. It is better to check 'top_index' and 'bottom_index' variables
      for real buffer size currently in use.
     */
    unsigned int buffer_size;

    //! Variable storing the highest index of buffer array in use.
    /*!
      The variable holds the highest index of buffer array, that is currently
      in use. This index does not need to be equal to the last index of buffer array.
     */
    unsigned int top_index;

    //! Variable storing the lowest index of buffer array in use.
    /*!
      The variable holds the lowest index of buffer array, that is currently
      in use. This index does not need to be equal to the very first index of buffer array.
     */
    unsigned int bottom_index;

    //! Index of byte that is ready to be replaced.
    /*!
      This variable stores the current index of byte (unsigned char) that is ready
      to be replaced. Each time new value is pushed, this value is incremented. If
      buffer is filled, this value is returned to the 'bottom_index' state.
     */
    unsigned int write_ptr;

    //! Index of byte that is ready to be read.
    /*!
      This variable stores the current index of byte (unsigned char) that is ready
      to be read. After the reading is done, this value is incremented. If reading
      reaches the last byte of buffer 'top_index', the reading moves back to 'bottom_index'.
     */
    unsigned int read_ptr;

};

#endif // CYCLICBUFFER_H
