/*
Tencent is pleased to support the open source community by making 
PhxPaxos available.
Copyright (C) 2016 THL A29 Limited, a Tencent company. 
All rights reserved.

Licensed under the BSD 3-Clause License (the "License"); you may 
not use this file except in compliance with the License. You may 
obtain a copy of the License at

https://opensource.org/licenses/BSD-3-Clause

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or 
implied. See the License for the specific language governing 
permissions and limitations under the License.

See the AUTHORS file for names of contributors. 
*/

#pragma once

#include "phxpaxos/def.h"
#include "phxpaxos/sm.h"
#include "phxpaxos/network.h"
#include "phxpaxos/storage.h"
#include "phxpaxos/log.h"
#include <vector>
#include <typeinfo>
#include <inttypes.h>
#include "breakpoint.h"

namespace phxpaxos
{

class LogStorage;
class NetWork;
class StateMachine;

typedef uint64_t nodeid_t;
static const nodeid_t nullnode = 0;

///////////////////////////////////////////////

class NodeInfo
{
public:
    NodeInfo(); 
    NodeInfo(const nodeid_t iNodeID); 
    NodeInfo(const std::string & sIP, const int iPort); 
    virtual ~NodeInfo() { }

    const nodeid_t GetNodeID() const;
    const std::string & GetIP() const;
    const int GetPort() const;

    void SetIPPort(const std::string & sIP, const int iPort);
    void SetNodeID(const nodeid_t iNodeID);

private:
    void MakeNodeID();
    void ParseNodeID();

    nodeid_t m_iNodeID;
    std::string m_sIP;
    int m_iPort;
};

class FollowerNodeInfo
{
public:
    NodeInfo oMyNode;
    NodeInfo oFollowNode;
};

typedef std::vector<NodeInfo> NodeInfoList;
typedef std::vector<FollowerNodeInfo> FollowerNodeInfoList;

/////////////////////////////////////////////////

class GroupSMInfo
{
public:
    GroupSMInfo();

    //required
    //GroupIdx interval is [0, iGroupCount) 
    int iGroupIdx;

    //optional
    //One paxos group can mounting multi state machines.
    std::vector<StateMachine *> vecSMList;

    //optional
    //Master election is a internal state machine. 
    //Set bIsUseMaster as true to open master election feature.
    //Default is false.
    bool bIsUseMaster;

};

typedef std::vector<GroupSMInfo> GroupSMInfoList;

/////////////////////////////////////////////////

typedef void (*MembershipChangeCallback)(const int iGroupIdx, NodeInfoList & vecNodeInfoList);

/////////////////////////////////////////////////

class Options
{
public:
    Options();

    //optional
    //User-specified paxoslog storage.
    //Default is nullptr.
    LogStorage * poLogStorage;

    //optional
    //If poLogStorage == nullptr, sLogStoragePath is required. 
    std::string sLogStoragePath;

    //optional
    //User-specified network.
    NetWork * poNetWork;

    //optional
    //Our default network use udp and tcp combination, a message we use udp or tcp to send decide by a threshold.
    //Message size under iUDPMaxSize we use udp to send.
    //Default is 2048.
    size_t iUDPMaxSize;
    
    //optional
    //We support to run multi phxpaxos on one process.
    //One paxos group here means one independent phxpaxos. Any two phxpaxos(paxos group) only share network, no other.
    //There is no communication between any two paxos group.
    //Default is 1.
    int iGroupCount;

    //required
    //Self node's ip/port.
    NodeInfo oMyNode;

    //required
    //All nodes's ip/port with a paxos set(usually three or five nodes).
    NodeInfoList vecNodeInfoList;

    //optional
    //One phxpaxos can mounting multi state machines.
    //This vector include different phxpaxos's state machines list.
    GroupSMInfoList vecGroupSMInfoList;

    //optional
    Breakpoint * poBreakpoint;

    //optional
    //If use this mode, that means you propose large value(maybe large than 5M means large) much more. 
    //Large value means long latency, long timeout, this mode will fit it.
    //Default is false
    bool bIsLargeValueMode;

    //optional
    //All followers's ip/port, and follow to node's ip/port.
    //Follower only learn but not participation paxos algorithmic process.
    //Default is empty.
    FollowerNodeInfoList vecFollowerNodeInfoList;

    //optional
    //Notice, this function must be thread safe!
    //if pLogFunc == nullptr, we will print log to standard ouput.
    LogFunc pLogFunc;

    //optional
    //If you use your own log function, then you control loglevel yourself, ignore this.
    //Check log.h to find 5 level.
    //Default is LogLevel::LogLevel_None, that means print no log.
    LogLevel eLogLevel;

    //optional
    //If you use checkpoint replayer feature, set as true.
    //Default is false;
    bool bUseCheckpointReplayer;
};
    
}
