/**
 * $Header$
 */

#ifndef WORLD_STATE_GUARD
#define WORLD_STATE_GUARD

#include <Config.hh>
#include <LocatedObject.hh>
#include <vector>
#include <Socket.hh>

/**
 * Stores a set of located objects - the world state
 */
template<int PAYLOAD_SIZE>
class WorldState {
private:
  std::vector<LocatedObject<PAYLOAD_SIZE>*> nodes;
public:
  WorldState();
  ~WorldState();
  void Add(LocatedObject<PAYLOAD_SIZE>* object);
  const std::vector<LocatedObject<PAYLOAD_SIZE>*>& GetNodes() { return nodes; }
    WorldState(Socket& socket);
    void Save(Socket& socket) const;
};

template<int PAYLOAD_SIZE> WorldState<PAYLOAD_SIZE>::WorldState() : nodes() {}

template<int PAYLOAD_SIZE> WorldState<PAYLOAD_SIZE>::~WorldState() {
  for(typename std::vector<LocatedObject<PAYLOAD_SIZE>*>::const_iterator i = nodes.begin();
      i != nodes.end();
      ++i) {
    delete *i;
  }
}

template<int PAYLOAD_SIZE> void WorldState<PAYLOAD_SIZE>::Add(LocatedObject<PAYLOAD_SIZE>* object) {
  nodes.push_back(object);
}

template<int PAYLOAD_SIZE> void WorldState<PAYLOAD_SIZE>::Save(Socket& socket) const {
    socket.Send(nodes.size());
    for(typename std::vector<LocatedObject<PAYLOAD_SIZE>*>::const_iterator i = nodes.begin();i!=nodes.end();++i) {
	(*i)->Save(socket);
    }
}

template<int PAYLOAD_SIZE> WorldState<PAYLOAD_SIZE>::WorldState(Socket& socket) {
    int count = socket.RecvInt();
    for(int i=0;i<count;++i) {
	nodes.push_back(new LocatedObject<PAYLOAD_SIZE>(socket));
    }
}


#endif//WORLD_STATE_GUARD
