/*
 * Copyright (c) 2020 ETH Zurich
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Simon               2020
 */

#include "arbiter-single-forward.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ArbiterSingleForward);
TypeId ArbiterSingleForward::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::ArbiterSingleForward")
            .SetParent<ArbiterSatnet> ()
            .SetGroupName("BasicSim")
    ;
    return tid;
}

ArbiterSingleForward::ArbiterSingleForward(
        Ptr<Node> this_node,
        NodeContainer nodes,
        std::vector<std::tuple<int32_t, int32_t, int32_t>> next_hop_list
) : ArbiterSatnet(this_node, nodes)
{
    m_next_hop_list = next_hop_list;
}

std::tuple<int32_t, int32_t, int32_t> ArbiterSingleForward::TopologySatelliteNetworkDecide(
        int32_t source_node_id,
        int32_t target_node_id,
        Ptr<const Packet> pkt,
        Ipv4Header const &ipHeader,
        bool is_request_for_source_ip_so_no_next_header
) {
    // return m_next_hop_list[target_node_id];
    // 取得原本 Hypatia 查表後的轉發條目
    auto entry = m_next_hop_list[target_node_id];

    // 使用 std::get 解析 tuple 中的詳細資訊
    int32_t next_node_id = std::get<0>(entry);
    int32_t own_if_id    = std::get<1>(entry);
    int32_t next_if_id   = std::get<2>(entry);

    // 在控制台印出當前路徑資訊
    std::cout
        << "Node = "      << m_node_id           // 當前節點 ID 
        << " target node = " << target_node_id      // 目標終點 ID 
        << " next node = "   << next_node_id        // 下一跳衛星 ID 
        << " my_if = "       << own_if_id           // 當前節點輸出的 Interface 
        << " next_if = "     << next_if_id          // 下一跳節點接收的 Interface 
        << std::endl; 

    // 回傳轉發決策結果
    return entry; 
}

void ArbiterSingleForward::SetSingleForwardState(int32_t target_node_id, int32_t next_node_id, int32_t own_if_id, int32_t next_if_id) {
    NS_ABORT_MSG_IF(next_node_id == -2 || own_if_id == -2 || next_if_id == -2, "Not permitted to set invalid (-2).");
    m_next_hop_list[target_node_id] = std::make_tuple(next_node_id, own_if_id, next_if_id);
}

std::string ArbiterSingleForward::StringReprOfForwardingState() {
    std::ostringstream res;
    res << "Single-forward state of node " << m_node_id << std::endl;
    for (size_t i = 0; i < m_nodes.GetN(); i++) {
        res << "  -> " << i << ": (" << std::get<0>(m_next_hop_list[i]) << ", "
            << std::get<1>(m_next_hop_list[i]) << ", "
            << std::get<2>(m_next_hop_list[i]) << ")" << std::endl;
    }
    return res.str();
}

}
