string local_db_logger::to_string(double double_value)
{
    std::ostringstream ss;
    ss << double_value;
    return ss.str();
}

string local_db_logger::to_string(int int_value)
{
    std::ostringstream ss;
    ss << int_value;
    return ss.str();
}