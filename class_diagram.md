# KD Project - Simplified UML

```mermaid
classDiagram
    %% Core Classes
    class Id {
        -bitset bits_
        +getBits()
    }

    class PeerInfo {
        +Id key_
        +endpoint_
        +last_seen_
    }

    %% Interfaces
    class INode {
        <<interface>>
        +find_K_closest()*
        +insert()*
        +get_id()*
    }

    class IPeer {
        <<interface>>
        +bootstrap()*
        +find()*
        +send()*
        +getNode()*
    }

    %% Implementations
    class Node {
        -Id id_
        -buckets_[K]
        +find_K_closest()
        +insert()
    }

    class Peer {
        -socket_
        -node_
        -lookups_
        +bootstrap()
        +find()
        +send()
    }

    %% Management
    class Swarm {
        -peers_
        -peers_map_
        +add()
        +getPeer()
        +async_getClosestPeer()
    }

    class LookupContext {
        -target_
        -peer_
        -node_
        -closest_peers_
        +start()
        +onResponse()
    }

    %% Relationships
    INode <|.. Node
    IPeer <|.. Peer
    
    Node *-- Id
    Node o-- PeerInfo
    Peer *-- INode
    Peer *-- PeerInfo
    Peer o-- LookupContext
    PeerInfo *-- Id
    Swarm o-- IPeer
    LookupContext --> IPeer
    LookupContext --> INode
```

## Основные компоненты

**Данные:**
- `Id` - идентификатор узла (bitset)
- `PeerInfo` - информация о пире (ID + endpoint)

**Интерфейсы:**
- `INode` - таблица маршрутизации
- `IPeer` - сетевой пир

**Реализации:**
- `Node` - k-buckets для DHT
- `Peer` - UDP-пир с сокетом

**Управление:**
- `Swarm` - менеджер всех пиров (singleton)
- `LookupContext` - итеративный поиск узлов
