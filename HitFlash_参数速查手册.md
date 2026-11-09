# HitFlash 着色参数速查手册

## 基础概念

| 参数 | 含义 | 范围 | 说明 |
|------|------|------|------|
| `R` | 红色分量 | 0.0~1.0 | 叠加主色调 |
| `G` | 绿色分量 | 0.0~1.0 | 辅助调色 |
| `B` | 蓝色分量 | 0.0~1.0 | 辅助调色 |
| `peakTime` | 爬升阶段(s) | 0.02~0.10 | 到达峰值强度的时间 |
| `fadeDuration` | 衰减阶段(s) | 0.10~0.50 | 从峰值衰减到0的时间 |
| `maxAlpha` | 峰值强度 | 0.3~1.0 | 0=不可见 1=完全覆盖 |

### 插值曲线示意

```
强度
max  |     /\
     |    /  \
     |   /    \_____ (二次缓出: 1 - t²)
0    |_________________→ 时间
     0  peak  peak+fade
```

- **上升段**：线性爬升 0 → maxAlpha
- **衰减段**：二次缓出 maxAlpha → 0
- **总时长**：`peakTime + fadeDuration`

---

## 使用方式

```cpp
// 场景初始化时预配置
auto flash = character->AddComponent<HitFlash>();
flash->SetFlashColor(R, G, B);
flash->SetFlashTiming(peakTime, fadeDuration, maxAlpha);

// 受击时触发
flash->TriggerHit();

// 每帧更新
flash->Update(deltaTime);
```

---

## 一、物理伤害系

### 1.1 暗红（苍月十字架 — 僵尸/Zombi）

```cpp
flash->SetFlashColor(0.60f, 0.05f, 0.05f);
flash->SetFlashTiming(0.04f, 0.25f, 0.75f);
```
> 深暗血红，短暂有力。主打苍月风格受击反馈，适合丧尸、骷髅等低级敌人。

### 1.2 标准红（泛用）

```cpp
flash->SetFlashColor(0.85f, 0.10f, 0.10f);
flash->SetFlashTiming(0.05f, 0.20f, 0.70f);
```
> 饱和度更高，肉眼更容易察觉。适合主角受击，比暗红更明显。

### 1.3 浅红（轻攻击）

```cpp
flash->SetFlashColor(0.70f, 0.30f, 0.30f);
flash->SetFlashTiming(0.03f, 0.15f, 0.50f);
```
> 轻快短暂，适合散射弹、多段攻击的连击闪烁。

### 1.4 深黑红（重击/Boss）

```cpp
flash->SetFlashColor(0.50f, 0.02f, 0.02f);
flash->SetFlashTiming(0.06f, 0.35f, 0.85f);
```
> 暗沉厚重，停留时间略长。锤击、蓄力斩等重型攻击专用。

---

## 二、元素伤害系

### 2.1 火焰 — 亮红偏橙

```cpp
flash->SetFlashColor(1.00f, 0.30f, 0.00f);
flash->SetFlashTiming(0.04f, 0.30f, 0.70f);
```
> 暖亮、扩散感。火球、爆炎、燃烧状态。

### 2.2 冰冻 — 浅蓝

```cpp
flash->SetFlashColor(0.15f, 0.50f, 0.95f);
flash->SetFlashTiming(0.04f, 0.35f, 0.65f);
```
> 清冷蓝调，衰减稍缓。冰锥、冻结、冰属性武器。

### 2.3 雷电 — 亮黄白

```cpp
flash->SetFlashColor(1.00f, 0.90f, 0.20f);
flash->SetFlashTiming(0.02f, 0.12f, 0.80f);
```
> 极速闪烁，雷电特有的瞬间感。雷击、电磁伤害。

### 2.4 暗/诅咒 — 深紫

```cpp
flash->SetFlashColor(0.50f, 0.05f, 0.55f);
flash->SetFlashTiming(0.05f, 0.30f, 0.75f);
```
> 暗紫压迫感。诅咒、暗属性、即死攻击。

### 2.5 毒/腐蚀 — 暗绿

```cpp
flash->SetFlashColor(0.20f, 0.55f, 0.10f);
flash->SetFlashTiming(0.05f, 0.35f, 0.65f);
```
> 暗绿色毒感。毒雾、腐蚀弹、持续伤害状态。

### 2.6 大地/石化 — 灰褐

```cpp
flash->SetFlashColor(0.40f, 0.30f, 0.15f);
flash->SetFlashTiming(0.06f, 0.30f, 0.70f);
```
> 泥土灰调。地震、石化、岩属性。

### 2.7 风/切割 — 亮白偏青

```cpp
flash->SetFlashColor(0.70f, 0.85f, 0.95f);
flash->SetFlashTiming(0.03f, 0.18f, 0.55f);
```
> 轻盈锐利，像刀光反照。风刃、切割类伤害。

---

## 三、神圣/治疗系

### 3.1 圣光 — 暖白

```cpp
flash->SetFlashColor(0.95f, 0.85f, 0.60f);
flash->SetFlashTiming(0.04f, 0.35f, 0.60f);
```
> 温暖柔和，金光感。圣属性、神圣武器。

### 3.2 治愈 — 浅绿白

```cpp
flash->SetFlashColor(0.70f, 0.95f, 0.70f);
flash->SetFlashTiming(0.05f, 0.40f, 0.45f);
```
> 淡绿回春感。治疗技能、回血。

### 3.3 护盾触发 — 淡蓝白

```cpp
flash->SetFlashColor(0.60f, 0.75f, 0.95f);
flash->SetFlashTiming(0.05f, 0.35f, 0.50f);
```
> 淡蓝魔法感。护盾抵消伤害、魔法防御触发。

---

## 四、特殊状态系

### 4.1 无敌/闪烁（持续 Blink）

```cpp
flash->SetFlashColor(1.00f, 1.00f, 1.00f);
flash->SetFlashTiming(0.08f, 0.15f, 0.40f);
```
> 浅白快速重复。配合 IsFlashing() 判断结束后再次 TriggerHit() 做循环闪烁。

### 4.2 击倒/眩晕 — 金星

```cpp
flash->SetFlashColor(1.00f, 0.80f, 0.05f);
flash->SetFlashTiming(0.05f, 0.40f, 0.60f);
```
> 金色晕眩。击倒、眩晕、昏厥状态。

### 4.3 暴击 — 脉冲白

```cpp
flash->SetFlashColor(0.95f, 0.95f, 0.95f);
flash->SetFlashTiming(0.03f, 0.20f, 0.85f);
```
> 亮白一闪。暴击专用，速度快强度高。

### 4.4 吸血 — 暗红偏紫

```cpp
flash->SetFlashColor(0.70f, 0.05f, 0.25f);
flash->SetFlashTiming(0.05f, 0.30f, 0.70f);
```
> 暗玫瑰色调。吸血技能、生命汲取。

### 4.5 怒气/狂暴 — 深橙

```cpp
flash->SetFlashColor(0.90f, 0.35f, 0.05f);
flash->SetFlashTiming(0.06f, 0.35f, 0.80f);
```
> 橘红爆发感。狂暴状态、愤怒技。

---

## 五、敌人分级

| 敌人等级 | 颜色 | R | G | B | peak | fade | maxA |
|---------|------|---|---|---|------|------|------|
| 杂兵 (Small) | 暗红 | 0.60 | 0.05 | 0.05 | 0.04 | 0.20 | 0.70 |
| 中型 (Medium) | 标准红 | 0.80 | 0.10 | 0.10 | 0.05 | 0.25 | 0.75 |
| 大型 (Large) | 深红 | 0.50 | 0.02 | 0.02 | 0.06 | 0.35 | 0.85 |
| Boss | 黑红 | 0.40 | 0.02 | 0.02 | 0.08 | 0.40 | 0.90 |
| 不死族 | 暗紫红 | 0.55 | 0.05 | 0.30 | 0.05 | 0.30 | 0.75 |
| 恶魔 | 暗红偏紫 | 0.65 | 0.05 | 0.35 | 0.05 | 0.30 | 0.80 |
| 装甲 | 灰红 | 0.55 | 0.20 | 0.20 | 0.05 | 0.28 | 0.70 |

---

## 六、调参经验

| 想要的效果 | 调整方向 |
|-----------|---------|
| 更"重"的打击感 | 提高 peakTime 到 0.05~0.06，让着色稍微"吃进去"再衰减 |
| 更轻快的闪烁 | 降低 fadeDuration 到 0.15~0.20 |
| 颜色更浓 | 提高 maxAlpha 到 0.85~0.90，但注意 0.9+ 会盖掉纹理 |
| 纹理更清晰 | 降低 maxAlpha 到 0.50~0.60 |
| 受击反馈更明显 | 提高 R 通道到 0.85+ 的同时提高 maxAlpha |
| 更暗沉 | R < 0.6，G/B < 0.1，适当提高 fadeDuration |
| 更明亮 | R > 0.8，G/B > 0.2，降低 fadeDuration |
| 低频重闪 | peakTime = 0.06~0.08，fadeDuration = 0.30~0.40 |
| 高频轻闪 | peakTime = 0.02~0.03，fadeDuration = 0.10~0.15 |

---

## 七、完整示例代码

```cpp
/// 角色基类中统一管理 HitFlash 配置
class Character : public GameObject
{
    HitFlash* mpFlash = nullptr;

protected:
    /// 按伤害类型配置闪烁参数
    void SetupHitFlash(DamageType type)
    {
        if (!mpFlash) mpFlash = AddComponent<HitFlash>();

        switch (type)
        {
        case DamageType::Physical:
            mpFlash->SetFlashColor(0.60f, 0.05f, 0.05f);
            mpFlash->SetFlashTiming(0.04f, 0.25f, 0.75f);
            break;
        case DamageType::Fire:
            mpFlash->SetFlashColor(1.00f, 0.30f, 0.00f);
            mpFlash->SetFlashTiming(0.04f, 0.30f, 0.70f);
            break;
        case DamageType::Ice:
            mpFlash->SetFlashColor(0.15f, 0.50f, 0.95f);
            mpFlash->SetFlashTiming(0.04f, 0.35f, 0.65f);
            break;
        case DamageType::Thunder:
            mpFlash->SetFlashColor(1.00f, 0.90f, 0.20f);
            mpFlash->SetFlashTiming(0.02f, 0.12f, 0.80f);
            break;
        case DamageType::Dark:
            mpFlash->SetFlashColor(0.50f, 0.05f, 0.55f);
            mpFlash->SetFlashTiming(0.05f, 0.30f, 0.75f);
            break;
        case DamageType::Poison:
            mpFlash->SetFlashColor(0.20f, 0.55f, 0.10f);
            mpFlash->SetFlashTiming(0.05f, 0.35f, 0.65f);
            break;
        case DamageType::Holy:
            mpFlash->SetFlashColor(0.95f, 0.85f, 0.60f);
            mpFlash->SetFlashTiming(0.04f, 0.35f, 0.60f);
            break;
        case DamageType::Critical:
            mpFlash->SetFlashColor(0.95f, 0.95f, 0.95f);
            mpFlash->SetFlashTiming(0.03f, 0.20f, 0.85f);
            break;
        }
    }

    /// 受击时调用
    void OnTakeDamage(int damage, DamageType type)
    {
        SetupHitFlash(type);
        mpFlash->TriggerHit();
        // ... 扣血逻辑
    }

public:
    void Update(float dt) override
    {
        if (mpFlash) mpFlash->Update(dt);
        // ... 其他逻辑
    }
};
```
