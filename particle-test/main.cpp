//
// Created by admin on 2026/1/9.
//

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <functional>
#include <string_id/string_id.hpp>
namespace ParticleSystem {

// ==================== 基础类型和枚举 ====================

// 模块使用掩码 - 指定模块在哪些脚本中使用
enum class ModuleUsageMask : uint32_t {
    None                = 0,
    Function            = 1 << 0,   // 函数
    Module              = 1 << 1,   // 模块
    DynamicInput       = 1 << 2,   // 动态输入
    ParticleGenerate   = 1 << 3,   // 粒子生成脚本
    ParticleUpdate     = 1 << 4,   // 粒子更新脚本
    ParticleEvent      = 1 << 5,   // 粒子事件脚本
    ParticleSimulate   = 1 << 6,   // 粒子模拟阶段脚本
    EmitterGenerate    = 1 << 7,   // 发射器生成脚本
    EmitterUpdate      = 1 << 8,   // 发射器更新脚本
    SystemGenerate     = 1 << 9,   // 系统生成脚本
    SystemUpdate       = 1 << 10   // 系统更新脚本
};

// 位掩码操作符
inline ModuleUsageMask operator|(ModuleUsageMask a, ModuleUsageMask b) {
    return static_cast<ModuleUsageMask>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline ModuleUsageMask operator&(ModuleUsageMask a, ModuleUsageMask b) {
    return static_cast<ModuleUsageMask>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline ModuleUsageMask& operator|=(ModuleUsageMask& a, ModuleUsageMask b) {
    a = a | b;
    return a;
}

// 依赖类型
enum class DependencyType {
    PROVIDED,   // 提供依赖
    REQUIRED    // 所需依赖
};

// ==================== 属性集 ====================

// 属性值类型
using PropertyValue = std::variant<
    int32_t,
    float,
    double,
    bool,
    std::string,
    std::vector<float>,
    std::vector<double>
>;

// 属性集基类
class PropertySet {
public:
    PropertySet() = default;
    virtual ~PropertySet() = default;

    // 获取属性值
    virtual PropertyValue get_property(const std::string& name) const = 0;

    // 设置属性值
    virtual void set_property(const std::string& name, const PropertyValue& value) = 0;

    // 检查属性是否存在
    virtual bool has_property(const std::string& name) const = 0;

    // 获取所有属性名
    virtual std::vector<std::string> get_property_names() const = 0;

protected:
    // 属性存储
    std::unordered_map<std::string, PropertyValue> properties_;
};

// 系统属性集
class SystemPropertySet : public PropertySet {
public:
    SystemPropertySet() = default;
    ~SystemPropertySet() override = default;

    PropertyValue get_property(const std::string& name) const override;
    void set_property(const std::string& name, const PropertyValue& value) override;
    bool has_property(const std::string& name) const override;
    std::vector<std::string> get_property_names() const override;
};

// 发射器属性集
class EmitterPropertySet : public PropertySet {
public:
    EmitterPropertySet() = default;
    ~EmitterPropertySet() override = default;

    PropertyValue get_property(const std::string& name) const override;
    void set_property(const std::string& name, const PropertyValue& value) override;
    bool has_property(const std::string& name) const override;
    std::vector<std::string> get_property_names() const override;
};

// 粒子属性集
class ParticlePropertySet : public PropertySet {
public:
    ParticlePropertySet() = default;
    ~ParticlePropertySet() override = default;

    PropertyValue get_property(const std::string& name) const override;
    void set_property(const std::string& name, const PropertyValue& value) override;
    bool has_property(const std::string& name) const override;
    std::vector<std::string> get_property_names() const override;
};

// ==================== 脚本执行上下文 ====================

class ScriptExecutionContext {
public:
    ScriptExecutionContext() = default;
    ~ScriptExecutionContext() = default;

    // 执行脚本
    virtual PropertyValue execute_script(
        const std::string& script_name,
        const std::vector<PropertyValue>& inputs
    ) = 0;

    // 注册脚本
    virtual void register_script(
        const std::string& name,
        std::function<PropertyValue(const std::vector<PropertyValue>&)> func
    ) = 0;

    // 获取属性集
    virtual std::shared_ptr<PropertySet> get_property_set() const = 0;
    virtual void set_property_set(std::shared_ptr<PropertySet> property_set) = 0;

private:
    std::shared_ptr<PropertySet> property_set_;
};

// ==================== 模块系统 ====================

// 属性依赖
struct PropertyDependency {
    std::string name_;
    DependencyType type_;
    std::string dependency_type_name_; // 依赖类型名称

    PropertyDependency(const std::string& name, DependencyType type, const std::string& type_name)
        : name_(name), type_(type), dependency_type_name_(type_name) {}
};

// 模块基类
class Module {
public:
    Module(const std::string& name, ModuleUsageMask usage_mask);
    virtual ~Module() = default;

    // 获取模块名称
    std::string get_name() const { return name_; }

    // 获取使用掩码
    ModuleUsageMask get_usage_mask() const { return usage_mask_; }

    // 添加属性依赖
    void add_dependency(const PropertyDependency& dependency);

    // 获取所有依赖
    const std::vector<PropertyDependency>& get_dependencies() const { return dependencies_; }

    // 执行模块
    virtual PropertyValue execute(
        std::shared_ptr<ScriptExecutionContext> context,
        const std::vector<PropertyValue>& inputs
    ) = 0;

protected:
    std::string name_;
    ModuleUsageMask usage_mask_;
    std::vector<PropertyDependency> dependencies_;
};

// 动态输入 - 具有单个输出值
class DynamicInput : public Module {
public:
    DynamicInput(const std::string& name);
    ~DynamicInput() override = default;

    // 设置输出值计算函数
    void set_output_calculator(std::function<PropertyValue(std::shared_ptr<ScriptExecutionContext>)> calculator);

    // 执行动态输入
    PropertyValue execute(
        std::shared_ptr<ScriptExecutionContext> context,
        const std::vector<PropertyValue>& inputs
    ) override;

private:
    std::function<PropertyValue(std::shared_ptr<ScriptExecutionContext>)> output_calculator_;
};

// 函数模块 - 辅助函数
class FunctionModule : public Module {
public:
    FunctionModule(const std::string& name);
    ~FunctionModule() override = default;

    // 设置函数实现
    void set_function(std::function<PropertyValue(const std::vector<PropertyValue>&)> func);

    // 执行函数
    PropertyValue execute(
        std::shared_ptr<ScriptExecutionContext> context,
        const std::vector<PropertyValue>& inputs
    ) override;

private:
    std::function<PropertyValue(const std::vector<PropertyValue>&)> function_;
};

// ==================== 发射器 ====================

class Emitter {
public:
    Emitter(const std::string& name);
    ~Emitter();

    // 获取发射器名称
    std::string get_name() const { return name_; }

    // 添加模块到堆栈
    void add_module(std::shared_ptr<Module> module);

    // 获取模块堆栈
    const std::vector<std::shared_ptr<Module>>& get_modules() const { return modules_; }

    // 发射器生成
    void generate(std::shared_ptr<ScriptExecutionContext> context);

    // 发射器更新
    void update(std::shared_ptr<ScriptExecutionContext> context, float delta_time);

    // 粒子生成
    void generate_particles(std::shared_ptr<ScriptExecutionContext> context, int count);

    // 粒子更新
    void update_particles(std::shared_ptr<ScriptExecutionContext> context, float delta_time);

    // 渲染
    void render(std::shared_ptr<ScriptExecutionContext> context);

    // 获取属性集
    std::shared_ptr<EmitterPropertySet> get_property_set() const { return property_set_; }

private:
    std::string name_;
    std::vector<std::shared_ptr<Module>> modules_;
    std::shared_ptr<EmitterPropertySet> property_set_;

    // 粒子数据
    struct Particle {
        std::shared_ptr<ParticlePropertySet> properties_;
        float lifetime_;
        float max_lifetime_;
    };

    std::vector<Particle> particles_;
};

// ==================== 粒子系统主类 ====================

class ParticleSystem {
public:
    ParticleSystem(const std::string& name);
    ~ParticleSystem();

    // 获取系统名称
    std::string get_name() const { return name_; }

    // 添加发射器
    void add_emitter(std::shared_ptr<Emitter> emitter);

    // 获取发射器列表
    const std::vector<std::shared_ptr<Emitter>>& get_emitters() const { return emitters_; }

    // 系统生成
    void generate(std::shared_ptr<ScriptExecutionContext> context);

    // 系统更新
    void update(std::shared_ptr<ScriptExecutionContext> context, float delta_time);

    // 渲染所有发射器
    void render(std::shared_ptr<ScriptExecutionContext> context);

    // 获取属性集
    std::shared_ptr<SystemPropertySet> get_property_set() const { return property_set_; }

    // 获取脚本执行上下文
    std::shared_ptr<ScriptExecutionContext> get_script_context() const { return script_context_; }

private:
    std::string name_;
    std::vector<std::shared_ptr<Emitter>> emitters_;
    std::shared_ptr<SystemPropertySet> property_set_;
    std::shared_ptr<ScriptExecutionContext> script_context_;
};

// ==================== 具体实现类 ====================

// 具体脚本执行上下文
class DefaultScriptExecutionContext : public ScriptExecutionContext {
public:
    DefaultScriptExecutionContext();
    ~DefaultScriptExecutionContext() override = default;

    PropertyValue execute_script(
        const std::string& script_name,
        const std::vector<PropertyValue>& inputs
    ) override;

    void register_script(
        const std::string& name,
        std::function<PropertyValue(const std::vector<PropertyValue>&)> func
    ) override;

    std::shared_ptr<PropertySet> get_property_set() const override { return property_set_; }
    void set_property_set(std::shared_ptr<PropertySet> property_set) override { property_set_ = property_set; }

private:
    std::unordered_map<
        std::string,
        std::function<PropertyValue(const std::vector<PropertyValue>&)>
    > scripts_;
    std::shared_ptr<PropertySet> property_set_;
};

// 示例模块：添加速度
class AddVelocityModule : public Module {
public:
    AddVelocityModule();
    ~AddVelocityModule() override = default;

    PropertyValue execute(
        std::shared_ptr<ScriptExecutionContext> context,
        const std::vector<PropertyValue>& inputs
    ) override;
};

// 示例动态输入：随机向量
class RandomVectorDynamicInput : public DynamicInput {
public:
    RandomVectorDynamicInput();
    ~RandomVectorDynamicInput() override = default;
};

} // namespace ParticleSystem

#endif // PARTICLE_SYSTEM_PROTOTYPE_H
