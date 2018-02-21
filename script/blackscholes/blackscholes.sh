rm -f ./trace/flow_blackscholes_xy_org.log
rm -f ./trace/router_blackschole_xy_org.log

../../cmp_org_huq_all_trace/sim-outorder \
        -redir:sim ./log/test_blackschole_xy_org.txt \
        -redir:dump ./log/test_blackschole_xy_org.out \
        -flow_trace ./trace/flow_blackschole_xy_org.log \
        -router_trace ./trace/router_blackschole_xy_org.log \
        -cpu:trace ./trace/cpu_blackschole_xy_org.log \
        -L2:trace ./trace/L2_blackschole_xy_org.log \
        -config ./config_mesh_ooo_xy_org \
		-max:barrier 0 -max:inst 100000 blackscholes64.BNC
