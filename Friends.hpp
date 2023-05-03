template<typename IFooAPI>
class Friends;
template<typename IFooAPI>
class Geographer;
template<typename IFooAPI>
class Pigeon;
template<typename IFooAPI>
class Predictor;
template<typename IFooAPI>
class CommandPost;

template<typename IFooAPI>
class CommandPost
{
    // 这是你和伙伴们所在的指挥所
public:
    // 这里有你们共享的信息

    IFooAPI &API;

    Geographer<IFooAPI> Alice;
    Predictor<IFooAPI> Bob;
    Pigeon<IFooAPI> Piggy;

    CommandPost(IFooAPI &api) : API(api), Alice(api, *this), Bob(api, *this), Piggy(api, *this) {}
    // 指挥所应当能够直接解决一些基本的问题而不劳烦几位专职人员，比如当前是否在箱子旁边，以及最近的作业位置等等
};

template<typename IFooAPI>
class Friends
{
    // Student&Tricker的伙伴们
protected:
    IFooAPI &API; // 方便起见，每个人都有对api的直接引用，其实可以直接用Center.API
    CommandPost<IFooAPI> &Center; // 你和伙伴们都可以访问自己所在的指挥所，并和其他人交流
    Friends(IFooAPI &api, CommandPost<IFooAPI> &Center_) : API(api), Center(Center_) {}
};

template<typename IFooAPI>
class Geographer : public Friends<IFooAPI>
{
    // 这是一位Geographer，负责告诉要怎么走
public:
    Geographer(IFooAPI &api, CommandPost<IFooAPI> &Center_) : Friends<IFooAPI>(api, Center_) {}
};

template<typename IFooAPI>
class Pigeon : public Friends<IFooAPI>
{
    // 这是一只Pigeon，负责传递信息
public:
    Pigeon(IFooAPI &api, CommandPost<IFooAPI> &Center_) : Friends<IFooAPI>(api, Center_) {}
};

template<typename IFooAPI>
class Predictor : public Friends<IFooAPI>
{
    // 这是一位Predictor，负责告诉其他人可能在哪里
public:
    Predictor(IFooAPI &api, CommandPost<IFooAPI> &Center_) : Friends<IFooAPI>(api, Center_) {}
};