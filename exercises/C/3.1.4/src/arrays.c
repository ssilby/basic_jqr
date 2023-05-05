
// these have been provided for the student
static void local_printf(const char * format, ...) 
{
  va_list args; 
  va_start( args, format);
  vdprintf( g_arrays_fd_stdout, format, args );
  va_end( args );
}

// these have been provided for the student
static int8_t print_array_entry(array_node_t * p_in_entry) 
{
  int8_t ret = -1;
  array_node_entry_t entry = p_in_entry->entry;
  if (NULL != p_in_entry) 
  {
        switch(p_in_entry->type)
    {

      case NODE_TYPE_INT16 :
        local_printf("%d", entry.int16);
      break;
      case NODE_TYPE_INT32 :
        local_printf("%d", entry.int32);
      break;
      case NODE_TYPE_INT64 :
        local_printf("%ld", entry.int64);
      break;
      
      case NODE_TYPE_C :
        local_printf("%c", entry.c);
      break;

      case NODE_TYPE_VOID_PTR : // both are pointer types
      case NODE_TYPE_C_PTR :
        local_printf("%p", entry.p_v);
      break;

      case NODE_TYPE_FLOAT : 
        local_printf("%f", entry.f);
      break;
      case NODE_TYPE_DOUBLE :
        local_printf("%f", entry.d);
      break;

      case NODE_TYPE_UINT16 :
        local_printf("%u", entry.uint16);
      break;
      case NODE_TYPE_UINT32 :
        local_printf("%u", entry.uint32);
      break;
      case NODE_TYPE_UINT64 :
        local_printf("%lu", entry.uint64);
      break;
      default :  // unknown type. do not print
        ret = -1;
    }
  }

  return ret;
}

