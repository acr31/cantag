/**
 * $Header$
 */

#ifndef WORLD_STATE_GUARD
#define WORLD_STATE_GUARD

#include <tripover/Config.hh>
#include <tripover/LocatedObject.hh>
#include <tripover/Socket.hh>

#include <vector>

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
  const std::vector<LocatedObject<PAYLOAD_SIZE>*>& GetNodes() const { return nodes; }
  WorldState(Socket& socket);
  int Save(Socket& socket) const;
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

template<int PAYLOAD_SIZE> int WorldState<PAYLOAD_SIZE>::Save(Socket& socket) const {
  int count = socket.Send(nodes.size());
  for(typename std::vector<LocatedObject<PAYLOAD_SIZE>*>::const_iterator i = nodes.begin();i!=nodes.end();++i) {
    count += (*i)->Save(socket);
  }
  return count;
}

template<int PAYLOAD_SIZE> WorldState<PAYLOAD_SIZE>::WorldState(Socket& socket) {
  int count = socket.RecvInt();
  for(int i=0;i<count;++i) {
    nodes.push_back(new LocatedObject<PAYLOAD_SIZE>(socket));
  }
}


#endif//WORLD_STATE_GUARD
