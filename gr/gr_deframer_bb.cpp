#include "gr_deframer_bb.h"

gr_deframer_bb_sptr make_gr_deframer_bb (int modem_type)
{
    return gnuradio::get_initial_sptr(new gr_deframer_bb(modem_type));
}

gr_deframer_bb::gr_deframer_bb(int modem_type) :
    gr::sync_block("gr_deframer_bb",
                   gr::io_signature::make (1, 1, sizeof (unsigned char)),
                   gr::io_signature::make (0, 0, 0))
{
    _offset = 0;
    _finished = false;
    _data = new std::vector<unsigned char>;
    _shift_reg = 0;
    _sync_found = false;
    _bit_buf_index = 0;
    _modem_type = modem_type;
    if(modem_type == 1)
    {
        _bit_buf_len = 8*8;
    }
    if(modem_type == 2)
    {
        _bit_buf_len = 4*8;
    }
}

gr_deframer_bb::~gr_deframer_bb()
{
    delete _data;
}

std::vector<unsigned char> * gr_deframer_bb::get_data()
{
    gr::thread::scoped_lock guard(_mutex);
    std::vector<unsigned char>* data = new std::vector<unsigned char>;
    data->reserve(_data->size());
    data->insert(data->end(),_data->begin(),_data->end());
    _data->clear();

    return data;
}


int gr_deframer_bb::findSync(unsigned char bit)
{


    _shift_reg = (_shift_reg << 1) | (bit & 0x1);
    u_int32_t temp;
    if(_modem_type != 2)
        temp = _shift_reg & 0xFFFF;
    else
        temp = _shift_reg & 0xFF;
    if((_modem_type == 2) && (temp == 0xB5))
    {
        _sync_found = true;
        return temp;
    }
    if((temp == 0x89ED))
    {
        _sync_found = true;
        return temp;
    }
    if((temp == 0xED89))
    {
        _sync_found = true;
        return temp;
    }
    if((temp == 0x98DE))
    {
        _sync_found = true;
        return temp;
    }
    if((temp == 0x8CC8))
    {

        _sync_found = true;
        return temp;
    }
    return 0;
}

int gr_deframer_bb::work(int noutput_items, gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items)
{
    if(noutput_items < 1)
    {
        usleep(1);
        return noutput_items;
    }
    unsigned char *in = (unsigned char*)(input_items[0]);
    for(int i=0;i < noutput_items;i++)
    {
        if(!_sync_found)
        {
            int current_frame_type = findSync(in[i]);
            if(_sync_found)
            {
                int bits;
                if(_modem_type == 1)
                {
                    bits = 16;
                }
                else
                {
                    bits = 8;
                }
                for(int k =0;k<bits;k++)
                {
                    gr::thread::scoped_lock guard(_mutex);
                    _data->push_back((unsigned char)((current_frame_type >> (bits-1-k)) & 0x1));
                }
                _bit_buf_index = 0;
                continue;
            }
        }
        if(_sync_found)
        {
            gr::thread::scoped_lock guard(_mutex);
            _data->push_back(in[i] & 0x1);
            _bit_buf_index++;
            if(_bit_buf_index >= _bit_buf_len)
            {
                _sync_found = false;
                _shift_reg = 0;
                _bit_buf_index = 0;
            }
        }
    }
    return noutput_items;
}