#include "Record.h"

unsigned int __global_record_id = 1;
Record::Record():m_id(__global_record_id++) { }
Record::Record(const Record& record) : m_id(__global_record_id++) {}
Record::Record(Record&& record):m_id(record.m_id){}
