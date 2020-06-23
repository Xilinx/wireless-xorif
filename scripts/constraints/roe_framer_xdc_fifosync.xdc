set from_list [get_pins -hier -regexp {.*ul_eth_pipe_i\/pkt_rdy_len_reg.*C}]
set to_list   [get_pins -hier -regexp {.*ul_eth_pipe_i\/pkt_rdy_len_sync_reg.+(?:CE|D)}]
set_false_path -from $from_list -to $to_list
#report_timing  -from $from_list -to $to_list -path_type summary -max_paths 10000
