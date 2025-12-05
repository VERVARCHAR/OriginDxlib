# -*- coding: utf-8 -*-
"""
Touhou-like STG JSON Editor (Boss / Stage / Talk)
"""

import json
import os
import tkinter as tk
from tkinter import ttk, messagebox, simpledialog, filedialog


# =====================================
# 共通ユーティリティ
# =====================================

def ensure_dir(path: str):
    if not os.path.isdir(path):
        os.makedirs(path, exist_ok=True)


def parse_typed_value(text: str, expected_type):
    """文字列から型変換。失敗したらそのまま文字列を返す。"""
    txt = text.strip()
    if txt == "":
        return expected_type() if expected_type in (int, float) else ""
    if expected_type is int:
        try:
            return int(txt)
        except ValueError:
            return txt
    if expected_type is float:
        try:
            return float(txt)
        except ValueError:
            return txt
    if expected_type is bool:
        lt = txt.lower()
        if lt in ("1", "true", "t", "on", "yes"):
            return True
        if lt in ("0", "false", "f", "off", "no"):
            return False
        return bool(txt)
    return text  # str 他


# =====================================
# デフォルトテンプレート
# =====================================

DEFAULT_SPELL = {
    "spellName": "",
    "spellType": 1,
    "isDurability": False,
    "durabilityTime": 60,
}

DEFAULT_BOSS = {
    "boss_name": "",
    "spells": [DEFAULT_SPELL.copy()],
}

DEFAULT_ENEMY = {
    "name": "",
    "id": 1,
    "type": 0,
    "lives": 1,
    "hp": 10,
    "radius": 12,
    "shootType": 0,
    "time": 0,
    "spawnTime": 60,
    "spellCount": 0,
    "isAlive": 0,
    "isSpell": 0,
    "isInvincible": 0,
    "invincibleTime": 0,
    "pos": {"x": 320.0, "y": -32.0},
    "vel": {"x": 0.0, "y": 2.0},
}

DEFAULT_STAGE = {
    "stage_name": "",
    "fps": 60,
    "bgm_name": "",
    "bgm_path": "",
    "enemies": [],
}

DEFAULT_TALK = {
    "talkStr": "",
    "isTalkEnemy": 0,
}

DEFAULT_TALK_STAGE = {
    "talks": [],
}


# =====================================
# Raw JSON Editor Dialog
# =====================================

class RawEditorDialog(tk.Toplevel):
    def __init__(self, master, obj, title="Edit JSON"):
        super().__init__(master)
        self.obj = obj
        self.title(title)

        self.text = tk.Text(self, width=80, height=25)
        self.text.pack(fill=tk.BOTH, expand=True)

        btn_f = ttk.Frame(self)
        btn_f.pack(fill=tk.X)
        ttk.Button(btn_f, text="OK", command=self.on_ok).pack(side=tk.RIGHT, padx=4, pady=4)
        ttk.Button(btn_f, text="Cancel", command=self.destroy).pack(side=tk.RIGHT, padx=4, pady=4)

        self.text.insert("1.0", json.dumps(self.obj, ensure_ascii=False, indent=4))

        self.grab_set()
        self.focus()

    def on_ok(self):
        txt = self.text.get("1.0", tk.END)
        try:
            new_obj = json.loads(txt)
        except Exception as e:
            messagebox.showerror("JSON Error", f"JSON のパースに失敗しました:\n{e}")
            return

        if isinstance(self.obj, dict):
            self.obj.clear()
            self.obj.update(new_obj)
        else:
            # dict 以外の場合は今回は想定外なので何もしない
            pass

        self.destroy()


# =====================================
# Boss Editor
# =====================================

class BossEditor(ttk.Frame):
    def __init__(self, master, app, *args, **kwargs):
        super().__init__(master, *args, **kwargs)
        self.app = app
        self.data = app.data  # {"bosses":[...]}
        self.current_boss_index = None
        self.current_spell_index = None

        self._build_ui()
        self.refresh_boss_list()

    # ---- flush ----
    def flush_changes(self):
        # Save ボタン時に呼ばれる
        self.save_current_spell_detail()
        self.save_current_boss_detail()

    # ---- data access ----
    @property
    def bosses(self):
        return self.data.setdefault("bosses", [])

    # ---- UI ----
    def _build_ui(self):
        left = ttk.Frame(self)
        left.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        ttk.Label(left, text="Boss List").pack(anchor=tk.W)
        self.boss_listbox = tk.Listbox(left, height=12)
        self.boss_listbox.pack(fill=tk.BOTH, expand=True)
        self.boss_listbox.bind("<<ListboxSelect>>", self.on_boss_selected)

        btn_frame = ttk.Frame(left)
        btn_frame.pack(fill=tk.X, pady=4)
        ttk.Button(btn_frame, text="Add Boss", command=self.add_boss).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Apply", command=self.apply_boss).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Delete", command=self.delete_boss).pack(side=tk.LEFT, padx=2)

        right = ttk.Frame(self)
        right.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Boss info
        bf = ttk.LabelFrame(right, text="Boss Info")
        bf.pack(fill=tk.X, pady=4)

        row = 0
        ttk.Label(bf, text="boss_name (str)").grid(row=row, column=0, sticky=tk.W)
        self.boss_name_var = tk.StringVar()
        ttk.Entry(bf, textvariable=self.boss_name_var, width=30).grid(row=row, column=1, sticky=tk.W)
        row += 1

        # Spells
        sf = ttk.LabelFrame(right, text="Spells")
        sf.pack(fill=tk.BOTH, expand=True, pady=4)

        left_s = ttk.Frame(sf)
        left_s.pack(side=tk.LEFT, fill=tk.Y, padx=2, pady=2)

        ttk.Label(left_s, text="Spell List").pack(anchor=tk.W)
        self.spell_listbox = tk.Listbox(left_s, height=8)
        self.spell_listbox.pack(fill=tk.BOTH, expand=True)
        self.spell_listbox.bind("<<ListboxSelect>>", self.on_spell_selected)

        sbtn_f = ttk.Frame(left_s)
        sbtn_f.pack(fill=tk.X, pady=4)
        ttk.Button(sbtn_f, text="Add", command=self.add_spell).pack(side=tk.LEFT, padx=2)
        ttk.Button(sbtn_f, text="Apply", command=self.apply_spell).pack(side=tk.LEFT, padx=2)
        ttk.Button(sbtn_f, text="Delete", command=self.delete_spell).pack(side=tk.LEFT, padx=2)
        ttk.Button(sbtn_f, text="Clone", command=self.clone_spell).pack(side=tk.LEFT, padx=2)

        right_s = ttk.Frame(sf)
        right_s.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=2, pady=2)

        row = 0
        ttk.Label(right_s, text="spellName (str)").grid(row=row, column=0, sticky=tk.W)
        self.spell_name_var = tk.StringVar()
        ttk.Entry(right_s, textvariable=self.spell_name_var, width=40).grid(row=row, column=1, sticky=tk.W)
        row += 1

        ttk.Label(right_s, text="spellType (int)").grid(row=row, column=0, sticky=tk.W)
        self.spell_type_var = tk.StringVar()
        ttk.Entry(right_s, textvariable=self.spell_type_var, width=10).grid(row=row, column=1, sticky=tk.W)
        row += 1

        self.is_durability_var = tk.BooleanVar()
        ttk.Checkbutton(
            right_s, text="isDurability (bool)", variable=self.is_durability_var
        ).grid(row=row, column=0, sticky=tk.W)
        row += 1

        ttk.Label(right_s, text="durabilityTime (int)").grid(row=row, column=0, sticky=tk.W)
        self.durability_time_var = tk.StringVar()
        ttk.Entry(right_s, textvariable=self.durability_time_var, width=10).grid(row=row, column=1, sticky=tk.W)
        row += 1

        raw_f = ttk.Frame(right)
        raw_f.pack(fill=tk.X, pady=4)
        ttk.Button(raw_f, text="Edit Raw JSON (Current Boss)", command=self.edit_raw_boss).pack(anchor=tk.W)

    # ---- list refresh ----
    def refresh_boss_list(self):
        sel_index = self.current_boss_index if self.current_boss_index is not None else 0
        self.boss_listbox.delete(0, tk.END)
        for i, boss in enumerate(self.bosses):
            name = boss.get("boss_name", f"boss{i+1}")
            self.boss_listbox.insert(tk.END, f"{i}: {name}")
        if self.bosses:
            if sel_index >= len(self.bosses):
                sel_index = len(self.bosses) - 1
            self.current_boss_index = sel_index
            self.boss_listbox.selection_set(sel_index)
        else:
            self.current_boss_index = None
            self.clear_boss_detail()
            self.refresh_spell_list()

        # 選択状態の Boss を詳細に反映
        self.show_current_boss_detail()

    def clear_boss_detail(self):
        self.boss_name_var.set("")
        self.clear_spell_detail()

    def show_current_boss_detail(self):
        """現在の Boss をフォームに反映（自動保存はしない）"""
        if self.current_boss_index is None or not self.bosses:
            self.clear_boss_detail()
            return
        boss = self.bosses[self.current_boss_index]
        self.boss_name_var.set(boss.get("boss_name", ""))
        # Spell リスト更新
        self.current_spell_index = None
        self.refresh_spell_list()

    def save_current_boss_detail(self):
        if self.current_boss_index is None or not self.bosses:
            return
        boss = self.bosses[self.current_boss_index]
        boss["boss_name"] = self.boss_name_var.get()

    # ---- spell ----
    def refresh_spell_list(self):
        sel_index = self.current_spell_index if self.current_spell_index is not None else 0
        self.spell_listbox.delete(0, tk.END)
        if self.current_boss_index is None or not self.bosses:
            self.current_spell_index = None
            self.clear_spell_detail()
            return
        spells = self.bosses[self.current_boss_index].setdefault("spells", [])
        for i, sp in enumerate(spells):
            name = sp.get("spellName", f"spell{i+1}")
            self.spell_listbox.insert(tk.END, f"{i}: {name}")
        if spells:
            if sel_index >= len(spells):
                sel_index = len(spells) - 1
            self.current_spell_index = sel_index
            self.spell_listbox.selection_set(sel_index)
        else:
            self.current_spell_index = None
            self.clear_spell_detail()

        self.show_current_spell_detail()

    def clear_spell_detail(self):
        self.spell_name_var.set("")
        self.spell_type_var.set("")
        self.is_durability_var.set(False)
        self.durability_time_var.set("")

    def show_current_spell_detail(self):
        if self.current_boss_index is None or self.current_spell_index is None:
            self.clear_spell_detail()
            return
        spells = self.bosses[self.current_boss_index].setdefault("spells", [])
        if not (0 <= self.current_spell_index < len(spells)):
            self.clear_spell_detail()
            return
        sp = spells[self.current_spell_index]
        self.spell_name_var.set(sp.get("spellName", ""))
        self.spell_type_var.set(str(sp.get("spellType", "")))
        self.is_durability_var.set(bool(sp.get("isDurability", False)))
        self.durability_time_var.set(str(sp.get("durabilityTime", "")))

    def save_current_spell_detail(self):
        if self.current_boss_index is None or self.current_spell_index is None:
            return
        spells = self.bosses[self.current_boss_index].setdefault("spells", [])
        if not (0 <= self.current_spell_index < len(spells)):
            return
        sp = spells[self.current_spell_index]
        sp["spellName"] = self.spell_name_var.get()
        sp["spellType"] = parse_typed_value(self.spell_type_var.get(), int)
        sp["isDurability"] = bool(self.is_durability_var.get())
        sp["durabilityTime"] = parse_typed_value(self.durability_time_var.get(), int)

    # ---- events ----
    def on_boss_selected(self, event=None):
        sel = self.boss_listbox.curselection()
        if not sel:
            return
        self.current_boss_index = sel[0]
        self.show_current_boss_detail()

    def on_spell_selected(self, event=None):
        sel = self.spell_listbox.curselection()
        if not sel:
            return
        self.current_spell_index = sel[0]
        self.show_current_spell_detail()

    # ---- operations ----
    def add_boss(self):
        self.save_current_boss_detail()
        self.bosses.append(json.loads(json.dumps(DEFAULT_BOSS)))
        self.current_boss_index = len(self.bosses) - 1
        self.refresh_boss_list()

    def apply_boss(self):
        self.save_current_boss_detail()
        self.refresh_boss_list()

    def delete_boss(self):
        if self.current_boss_index is None:
            return
        if not messagebox.askyesno("Delete Boss", "選択中の Boss を削除しますか？"):
            return
        self.bosses.pop(self.current_boss_index)
        self.current_boss_index = None
        self.refresh_boss_list()

    def add_spell(self):
        if self.current_boss_index is None:
            return
        self.save_current_spell_detail()
        boss = self.bosses[self.current_boss_index]
        spells = boss.setdefault("spells", [])
        spells.append(json.loads(json.dumps(DEFAULT_SPELL)))
        self.current_spell_index = len(spells) - 1
        self.refresh_spell_list()

    def apply_spell(self):
        self.save_current_spell_detail()
        self.refresh_spell_list()

    def delete_spell(self):
        if self.current_boss_index is None or self.current_spell_index is None:
            return
        boss = self.bosses[self.current_boss_index]
        spells = boss.setdefault("spells", [])
        if not (0 <= self.current_spell_index < len(spells)):
            return
        if not messagebox.askyesno("Delete Spell", "選択中の Spell を削除しますか？"):
            return
        spells.pop(self.current_spell_index)
        self.current_spell_index = None
        self.refresh_spell_list()

    def clone_spell(self):
        if self.current_boss_index is None or self.current_spell_index is None:
            return
        boss = self.bosses[self.current_boss_index]
        spells = boss.setdefault("spells", [])
        src = spells[self.current_spell_index]
        spells.append(json.loads(json.dumps(src)))
        self.current_spell_index = len(spells) - 1
        self.refresh_spell_list()

    def edit_raw_boss(self):
        if self.current_boss_index is None:
            messagebox.showinfo("Info", "Boss が選択されていません。")
            return
        self.save_current_boss_detail()
        boss = self.bosses[self.current_boss_index]
        RawEditorDialog(self, boss, title="Edit Raw Boss JSON")
        self.refresh_boss_list()



# =====================================
# Stage Editor
# =====================================




class BulkAddDialog(simpledialog.Dialog):
    """
    Bulk Add 用: 等差的に変化させるキーと増分をまとめて指定するダイアログ
    """
    def __init__(self, parent, key_paths):
        self.key_paths = key_paths
        self.result = None  # {path: step, ...} or None
        super().__init__(parent, title="Bulk Add - 等差設定")

    def body(self, master):
        ttk.Label(
            master,
            text="等差的に変化させるキーにチェックを入れ、1個ごとの増分を入力してください。"
        ).grid(row=0, column=0, columnspan=2, sticky="w", padx=4, pady=4)

        self._use_vars = {}
        self._step_vars = {}

        row = 1
        for path in self.key_paths:
            use_var = tk.BooleanVar(value=False)
            step_var = tk.StringVar(value="")

            chk = ttk.Checkbutton(master, text=path, variable=use_var)
            chk.grid(row=row, column=0, sticky="w", padx=4, pady=2)

            ent = ttk.Entry(master, textvariable=step_var, width=10)
            ent.grid(row=row, column=1, sticky="w", padx=4, pady=2)

            self._use_vars[path] = use_var
            self._step_vars[path] = step_var
            row += 1

        return master

    def validate(self):
        selected = {}
        for path in self.key_paths:
            if self._use_vars[path].get():
                s = self._step_vars[path].get().strip()
                if not s:
                    messagebox.showerror(
                        "Bulk Add",
                        f"キー '{path}' の増分を入力してください。",
                        parent=self,
                    )
                    return False
                try:
                    step = float(s)
                except ValueError:
                    messagebox.showerror(
                        "Bulk Add",
                        f"キー '{path}' の増分 '{s}' を数値として解釈できません。",
                        parent=self,
                    )
                    return False
                selected[path] = step

        # 何も選ばなくても OK（その場合は単純クローン）
        self._selected = selected
        return True

    def apply(self):
        # validate() で作った dict をそのまま返す
        self.result = getattr(self, "_selected", {})

class StageEditor(ttk.Frame):
    def __init__(self, master, app, *args, **kwargs):
        super().__init__(master, *args, **kwargs)
        self.app = app
        self.data = app.data  # {"stages":[...]}
        self.current_stage_index = None
        self.current_enemy_index = None

        self._build_ui()
        self.refresh_stage_list()

    # ---- flush ----
    def flush_changes(self):
        self.save_current_enemy_detail()
        self.save_current_stage_detail()

    # ---- data ----
    @property
    def stages(self):
        return self.data.setdefault("stages", [])

    def enemies_of_current_stage(self):
        if self.current_stage_index is None:
            return []
        st = self.stages[self.current_stage_index]
        return st.setdefault("enemies", [])

    # ---- helper: unique enemy id ----
    def _assign_unique_id_for_enemy(self, enemy, enemies, skip_index=None):
        """enemy['id'] にユニークな id を割り当てる（既に値がある場合はそれをベースに調整）"""
        used_ids = set()
        for i, e in enumerate(enemies):
            if skip_index is not None and i == skip_index:
                continue
            eid = e.get("id")
            if isinstance(eid, int):
                used_ids.add(eid)

        base_id = enemy.get("id")
        if not isinstance(base_id, int):
            base_id = 1
        new_id = base_id
        while new_id in used_ids:
            new_id += 1
        enemy["id"] = new_id

    # ---- UI ----
    def _build_ui(self):
        left = ttk.Frame(self)
        left.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        ttk.Label(left, text="Stage List").pack(anchor=tk.W)
        self.stage_listbox = tk.Listbox(left, height=12)
        self.stage_listbox.pack(fill=tk.BOTH, expand=True)
        self.stage_listbox.bind("<<ListboxSelect>>", self.on_stage_selected)

        btn_f = ttk.Frame(left)
        btn_f.pack(fill=tk.X, pady=4)
        ttk.Button(btn_f, text="Add Stage", command=self.add_stage).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_f, text="Apply", command=self.apply_stage).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_f, text="Delete", command=self.delete_stage).pack(side=tk.LEFT, padx=2)

        right = ttk.Frame(self)
        right.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Stage Info
        sf = ttk.LabelFrame(right, text="Stage Info")
        sf.pack(fill=tk.X, pady=4)

        row = 0
        ttk.Label(sf, text="stage_name (str)").grid(row=row, column=0, sticky=tk.W)
        self.stage_name_var = tk.StringVar()
        ttk.Entry(sf, textvariable=self.stage_name_var, width=30).grid(row=row, column=1, sticky=tk.W)
        row += 1

        ttk.Label(sf, text="fps (int)").grid(row=row, column=0, sticky=tk.W)
        self.stage_fps_var = tk.StringVar()
        ttk.Entry(sf, textvariable=self.stage_fps_var, width=10).grid(row=row, column=1, sticky=tk.W)
        row += 1

        ttk.Label(sf, text="bgm_name (str)").grid(row=row, column=0, sticky=tk.W)
        self.stage_bgm_name_var = tk.StringVar()
        ttk.Entry(sf, textvariable=self.stage_bgm_name_var, width=30).grid(row=row, column=1, sticky=tk.W)
        row += 1

        ttk.Label(sf, text="bgm_path (str)").grid(row=row, column=0, sticky=tk.W)
        self.stage_bgm_path_var = tk.StringVar()
        ttk.Entry(sf, textvariable=self.stage_bgm_path_var, width=40).grid(row=row, column=1, sticky=tk.W)
        row += 1

        # Enemies
        ef = ttk.LabelFrame(right, text="Enemies")
        ef.pack(fill=tk.BOTH, expand=True, pady=4)

        left_e = ttk.Frame(ef)
        left_e.pack(side=tk.LEFT, fill=tk.Y, padx=2, pady=2)

        ttk.Label(left_e, text="Enemy List").pack(anchor=tk.W)
        self.enemy_listbox = tk.Listbox(left_e, height=10)
        self.enemy_listbox.pack(fill=tk.BOTH, expand=True)
        self.enemy_listbox.bind("<<ListboxSelect>>", self.on_enemy_selected)

        ebtn_f = ttk.Frame(left_e)
        ebtn_f.pack(fill=tk.X, pady=4)
        ttk.Button(ebtn_f, text="Add", command=self.add_enemy).pack(side=tk.LEFT, padx=2)
        ttk.Button(ebtn_f, text="Apply", command=self.apply_enemy).pack(side=tk.LEFT, padx=2)
        ttk.Button(ebtn_f, text="Delete", command=self.delete_enemy).pack(side=tk.LEFT, padx=2)
        ttk.Button(ebtn_f, text="Clone", command=self.clone_enemy).pack(side=tk.LEFT, padx=2)
        ttk.Button(ebtn_f, text="Bulk Add", command=self.bulk_add_enemy).pack(side=tk.LEFT, padx=2)

        right_e = ttk.Frame(ef)
        right_e.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=2, pady=2)

        row = 0

        def add_labeled_entry(label, var, width=10):
            nonlocal row
            ttk.Label(right_e, text=label).grid(row=row, column=0, sticky=tk.W)
            ttk.Entry(right_e, textvariable=var, width=width).grid(row=row, column=1, sticky=tk.W)
            row += 1

        self.enemy_name_var = tk.StringVar()
        add_labeled_entry("name (str)", self.enemy_name_var, width=20)

        self.enemy_id_var = tk.StringVar()
        add_labeled_entry("id (int, unique)", self.enemy_id_var)

        self.enemy_type_var = tk.StringVar()
        add_labeled_entry("type (int)", self.enemy_type_var)

        self.enemy_lives_var = tk.StringVar()
        add_labeled_entry("lives (int)", self.enemy_lives_var)

        self.enemy_hp_var = tk.StringVar()
        add_labeled_entry("hp (int)", self.enemy_hp_var)

        self.enemy_radius_var = tk.StringVar()
        add_labeled_entry("radius (int)", self.enemy_radius_var)

        self.enemy_shoot_type_var = tk.StringVar()
        add_labeled_entry("shootType (int)", self.enemy_shoot_type_var)

        self.enemy_time_var = tk.StringVar()
        add_labeled_entry("time (int)", self.enemy_time_var)

        self.enemy_spawn_time_var = tk.StringVar()
        add_labeled_entry("spawnTime (int)", self.enemy_spawn_time_var)

        self.enemy_spell_count_var = tk.StringVar()
        add_labeled_entry("spellCount (int)", self.enemy_spell_count_var)

        # isAlive / isSpell / isInvincible はチェックボックス (0/1 保存)
        self.enemy_is_alive_var = tk.BooleanVar()
        ttk.Checkbutton(right_e, text="isAlive (0/1)", variable=self.enemy_is_alive_var).grid(
            row=row, column=0, sticky=tk.W
        )
        row += 1

        self.enemy_is_spell_var = tk.BooleanVar()
        ttk.Checkbutton(right_e, text="isSpell (0/1)", variable=self.enemy_is_spell_var).grid(
            row=row, column=0, sticky=tk.W
        )
        row += 1

        self.enemy_is_invincible_var = tk.BooleanVar()
        ttk.Checkbutton(right_e, text="isInvincible (0/1)", variable=self.enemy_is_invincible_var).grid(
            row=row, column=0, sticky=tk.W
        )
        row += 1

        self.enemy_invincible_time_var = tk.StringVar()
        add_labeled_entry("invincibleTime (int)", self.enemy_invincible_time_var)

        ttk.Label(right_e, text="pos.x (float)").grid(row=row, column=0, sticky=tk.W)
        self.enemy_pos_x_var = tk.StringVar()
        ttk.Entry(right_e, textvariable=self.enemy_pos_x_var, width=10).grid(row=row, column=1, sticky=tk.W)
        row += 1

        ttk.Label(right_e, text="pos.y (float)").grid(row=row, column=0, sticky=tk.W)
        self.enemy_pos_y_var = tk.StringVar()
        ttk.Entry(right_e, textvariable=self.enemy_pos_y_var, width=10).grid(row=row, column=1, sticky=tk.W)
        row += 1

        ttk.Label(right_e, text="vel.x (float)").grid(row=row, column=0, sticky=tk.W)
        self.enemy_vel_x_var = tk.StringVar()
        ttk.Entry(right_e, textvariable=self.enemy_vel_x_var, width=10).grid(row=row, column=1, sticky=tk.W)
        row += 1

        ttk.Label(right_e, text="vel.y (float)").grid(row=row, column=0, sticky=tk.W)
        self.enemy_vel_y_var = tk.StringVar()
        ttk.Entry(right_e, textvariable=self.enemy_vel_y_var, width=10).grid(row=row, column=1, sticky=tk.W)
        row += 1

        raw_f = ttk.Frame(right)
        raw_f.pack(fill=tk.X, pady=4)
        ttk.Button(raw_f, text="Edit Raw JSON (Current Enemy)", command=self.edit_raw_enemy).pack(anchor=tk.W)

    # ---- Stage ----
    def refresh_stage_list(self):
        sel_index = self.current_stage_index if self.current_stage_index is not None else 0
        self.stage_listbox.delete(0, tk.END)
        for i, st in enumerate(self.stages):
            name = st.get("stage_name", f"Stage{i+1}")
            self.stage_listbox.insert(tk.END, f"{i}: {name}")
        if self.stages:
            if sel_index >= len(self.stages):
                sel_index = len(self.stages) - 1
            self.current_stage_index = sel_index
            self.stage_listbox.selection_set(sel_index)
        else:
            self.current_stage_index = None
            self.clear_stage_detail()
            self.refresh_enemy_list()

        self.show_current_stage_detail()

    def clear_stage_detail(self):
        self.stage_name_var.set("")
        self.stage_fps_var.set("")
        self.stage_bgm_name_var.set("")
        self.stage_bgm_path_var.set("")

    def show_current_stage_detail(self):
        if self.current_stage_index is None or not self.stages:
            self.clear_stage_detail()
            return
        st = self.stages[self.current_stage_index]
        self.stage_name_var.set(st.get("stage_name", ""))
        self.stage_fps_var.set(str(st.get("fps", "")))
        self.stage_bgm_name_var.set(st.get("bgm_name", ""))
        self.stage_bgm_path_var.set(st.get("bgm_path", ""))
        # Enemy リスト更新
        self.current_enemy_index = None
        self.refresh_enemy_list()

    def save_current_stage_detail(self):
        if self.current_stage_index is None or not self.stages:
            return
        st = self.stages[self.current_stage_index]
        st["stage_name"] = self.stage_name_var.get()
        st["fps"] = parse_typed_value(self.stage_fps_var.get(), int)
        st["bgm_name"] = self.stage_bgm_name_var.get()
        st["bgm_path"] = self.stage_bgm_path_var.get()

    def on_stage_selected(self, event=None):
        sel = self.stage_listbox.curselection()
        if not sel:
            return
        self.current_stage_index = sel[0]
        self.show_current_stage_detail()

    def add_stage(self):
        self.save_current_stage_detail()
        self.stages.append(json.loads(json.dumps(DEFAULT_STAGE)))
        self.current_stage_index = len(self.stages) - 1
        self.refresh_stage_list()

    def apply_stage(self):
        self.save_current_stage_detail()
        self.refresh_stage_list()

    def delete_stage(self):
        if self.current_stage_index is None:
            return
        if not messagebox.askyesno("Delete Stage", "選択中の Stage を削除しますか？"):
            return
        self.stages.pop(self.current_stage_index)
        self.current_stage_index = None
        self.refresh_stage_list()

    # ---- Enemy ----
    def refresh_enemy_list(self):
        sel_index = self.current_enemy_index if self.current_enemy_index is not None else 0
        self.enemy_listbox.delete(0, tk.END)
        enemies = self.enemies_of_current_stage()
        for i, e in enumerate(enemies):
            name = e.get("name", f"enemy{i+1}")
            spawn = e.get("spawnTime", 0)
            self.enemy_listbox.insert(tk.END, f"{i}: t={spawn} id={e.get('id', '?')} {name}")
        if enemies:
            if sel_index >= len(enemies):
                sel_index = len(enemies) - 1
            self.current_enemy_index = sel_index
            self.enemy_listbox.selection_set(sel_index)
        else:
            self.current_enemy_index = None
            self.clear_enemy_detail()

        self.show_current_enemy_detail()

    def clear_enemy_detail(self):
        self.enemy_name_var.set("")
        self.enemy_id_var.set("")
        self.enemy_type_var.set("")
        self.enemy_lives_var.set("")
        self.enemy_hp_var.set("")
        self.enemy_radius_var.set("")
        self.enemy_shoot_type_var.set("")
        self.enemy_time_var.set("")
        self.enemy_spawn_time_var.set("")
        self.enemy_spell_count_var.set("")
        self.enemy_is_alive_var.set(False)
        self.enemy_is_spell_var.set(False)
        self.enemy_is_invincible_var.set(False)
        self.enemy_invincible_time_var.set("")
        self.enemy_pos_x_var.set("")
        self.enemy_pos_y_var.set("")
        self.enemy_vel_x_var.set("")
        self.enemy_vel_y_var.set("")

    def show_current_enemy_detail(self):
        if self.current_stage_index is None or self.current_enemy_index is None:
            self.clear_enemy_detail()
            return
        enemies = self.enemies_of_current_stage()
        if not (0 <= self.current_enemy_index < len(enemies)):
            self.clear_enemy_detail()
            return
        e = enemies[self.current_enemy_index]

        self.enemy_name_var.set(e.get("name", ""))
        self.enemy_id_var.set(str(e.get("id", "")))
        self.enemy_type_var.set(str(e.get("type", "")))
        self.enemy_lives_var.set(str(e.get("lives", "")))
        self.enemy_hp_var.set(str(e.get("hp", "")))
        self.enemy_radius_var.set(str(e.get("radius", "")))
        self.enemy_shoot_type_var.set(str(e.get("shootType", "")))
        self.enemy_time_var.set(str(e.get("time", "")))
        self.enemy_spawn_time_var.set(str(e.get("spawnTime", "")))
        self.enemy_spell_count_var.set(str(e.get("spellCount", "")))

        self.enemy_is_alive_var.set(bool(e.get("isAlive", 0)))
        self.enemy_is_spell_var.set(bool(e.get("isSpell", 0)))
        self.enemy_is_invincible_var.set(bool(e.get("isInvincible", 0)))

        self.enemy_invincible_time_var.set(str(e.get("invincibleTime", "")))

        pos = e.get("pos", {})
        vel = e.get("vel", {})
        self.enemy_pos_x_var.set(str(pos.get("x", "")))
        self.enemy_pos_y_var.set(str(pos.get("y", "")))
        self.enemy_vel_x_var.set(str(vel.get("x", "")))
        self.enemy_vel_y_var.set(str(vel.get("y", "")))

    def save_current_enemy_detail(self):
        if self.current_stage_index is None or self.current_enemy_index is None:
            return
        enemies = self.enemies_of_current_stage()
        if not (0 <= self.current_enemy_index < len(enemies)):
            return
        e = enemies[self.current_enemy_index]

        e["name"] = self.enemy_name_var.get()

        # id（ユニーク）
        new_id = parse_typed_value(self.enemy_id_var.get(), int)
        if not isinstance(new_id, int):
            new_id = e.get("id", 1)
        e["id"] = new_id
        # ユニークになるよう調整
        self._assign_unique_id_for_enemy(e, enemies, skip_index=self.current_enemy_index)

        e["type"] = parse_typed_value(self.enemy_type_var.get(), int)
        e["lives"] = parse_typed_value(self.enemy_lives_var.get(), int)
        e["hp"] = parse_typed_value(self.enemy_hp_var.get(), int)
        e["radius"] = parse_typed_value(self.enemy_radius_var.get(), int)
        e["shootType"] = parse_typed_value(self.enemy_shoot_type_var.get(), int)
        e["time"] = parse_typed_value(self.enemy_time_var.get(), int)
        e["spawnTime"] = parse_typed_value(self.enemy_spawn_time_var.get(), int)
        e["spellCount"] = parse_typed_value(self.enemy_spell_count_var.get(), int)

        # フラグは 0/1 として保存
        e["isAlive"] = 1 if self.enemy_is_alive_var.get() else 0
        e["isSpell"] = 1 if self.enemy_is_spell_var.get() else 0
        e["isInvincible"] = 1 if self.enemy_is_invincible_var.get() else 0

        e["invincibleTime"] = parse_typed_value(self.enemy_invincible_time_var.get(), int)

        pos = e.setdefault("pos", {})
        pos["x"] = parse_typed_value(self.enemy_pos_x_var.get(), float)
        pos["y"] = parse_typed_value(self.enemy_pos_y_var.get(), float)
        vel = e.setdefault("vel", {})
        vel["x"] = parse_typed_value(self.enemy_vel_x_var.get(), float)
        vel["y"] = parse_typed_value(self.enemy_vel_y_var.get(), float)

    def on_enemy_selected(self, event=None):
        sel = self.enemy_listbox.curselection()
        if not sel:
            return
        self.current_enemy_index = sel[0]
        self.show_current_enemy_detail()

    def add_enemy(self):
        if self.current_stage_index is None:
            return
        self.save_current_enemy_detail()
        enemies = self.enemies_of_current_stage()
        new_enemy = json.loads(json.dumps(DEFAULT_ENEMY))
        enemies.append(new_enemy)
        self._assign_unique_id_for_enemy(new_enemy, enemies, skip_index=len(enemies) - 1)
        self.current_enemy_index = len(enemies) - 1
        self.refresh_enemy_list()

    def apply_enemy(self):
        self.save_current_enemy_detail()
        self.refresh_enemy_list()

    def delete_enemy(self):
        if self.current_stage_index is None or self.current_enemy_index is None:
            return
        enemies = self.enemies_of_current_stage()
        if not (0 <= self.current_enemy_index < len(enemies)):
            return
        if not messagebox.askyesno("Delete Enemy", "選択中の Enemy を削除しますか？"):
            return
        enemies.pop(self.current_enemy_index)
        self.current_enemy_index = None
        self.refresh_enemy_list()

    def clone_enemy(self):
        if self.current_stage_index is None or self.current_enemy_index is None:
            return
        enemies = self.enemies_of_current_stage()
        src = enemies[self.current_enemy_index]
        new_enemy = json.loads(json.dumps(src))
        enemies.append(new_enemy)
        self._assign_unique_id_for_enemy(new_enemy, enemies, skip_index=len(enemies) - 1)
        self.current_enemy_index = len(enemies) - 1
        self.refresh_enemy_list()

    # ---- helper: nested key access ----
    def _get_value_by_path(self, obj, path: str):
        """
        path: 'pos.x' や 'spawnTime' のようなドット区切りで
        ネストした値を取得する。
        """
        keys = path.split(".")
        cur = obj
        for k in keys:
            if isinstance(cur, dict) and k in cur:
                cur = cur[k]
            else:
                return None
        return cur

    def _set_value_by_path(self, obj, path: str, value):
        """
        path: 'pos.x' や 'spawnTime' のようなドット区切りで
        ネストした値を設定する。
        中間の dict が無ければ作る。
        """
        keys = path.split(".")
        cur = obj
        for k in keys[:-1]:
            if not isinstance(cur, dict):
                # dict じゃなかったら上書きして dict にする（通常は起きない想定）
                cur = {}
            cur = cur.setdefault(k, {})
        cur[keys[-1]] = value

    def _collect_numeric_paths(self, obj, prefix=""):
        """
        テンプレート Enemy から「数値として扱えるキー」のパス一覧を集める。
        例: 'time', 'spawnTime', 'pos.x', 'vel.y' など。
        """
        paths = []
        if isinstance(obj, dict):
            for k, v in obj.items():
                p = f"{prefix}.{k}" if prefix else k
                if isinstance(v, (int, float)):
                    # id は自動で振り直しなので除外
                    if not (prefix == "" and k == "id"):
                        paths.append(p)
                elif isinstance(v, dict):
                    paths.extend(self._collect_numeric_paths(v, p))
        return paths

    def bulk_add_enemy(self):
        if self.current_stage_index is None:
            return

        # 何個増やすか
        n = simpledialog.askinteger(
            "Bulk Add", "追加する Enemy 数を入力してください:",
            minvalue=1, maxvalue=1000
        )
        if not n:
            return

        enemies = self.enemies_of_current_stage()

        # テンプレート（選択中があればそれ、なければ DEFAULT_ENEMY）
        if self.current_enemy_index is not None and 0 <= self.current_enemy_index < len(enemies):
            template = enemies[self.current_enemy_index]
        else:
            template = DEFAULT_ENEMY

        # テンプレートから「数値のキー」のパス一覧を集める
        key_paths = self._collect_numeric_paths(template)
        if not key_paths:
            messagebox.showinfo("Bulk Add", "数値として扱えるキーが見つかりません。")
            return

        # ダイアログで「どのキーを等差的に変化させるか」とその増分を指定
        dlg = BulkAddDialog(self, key_paths)
        if dlg.result is None:
            # キャンセルされた
            return

        selected_steps = dlg.result  # {path: step(float), ...}

        # 何も選んでいない場合は単純クローン
        use_arith = bool(selected_steps)

        # 各キーごとに base 値と型情報を記録
        arith_info = {}
        if use_arith:
            for path, step in selected_steps.items():
                base_value = self._get_value_by_path(template, path)
                if not isinstance(base_value, (int, float)):
                    # _collect_numeric_paths で数値に限定しているので通常ここには来ない想定
                    messagebox.showerror(
                        "Bulk Add",
                        f"キー '{path}' の元の値が数値ではありません: {base_value!r}",
                    )
                    return
                arith_info[path] = {
                    "base": base_value,
                    "step": step,
                    "is_int": isinstance(base_value, int),
                }

        # 実際の追加処理
        for i in range(n):
            # テンプレートのディープコピー
            new_enemy = json.loads(json.dumps(template))

            # 等差的な変更（複数キー対応）
            if use_arith:
                for path, info in arith_info.items():
                    new_val = info["base"] + info["step"] * (i + 1)
                    if info["is_int"]:
                        new_val = int(new_val)
                    self._set_value_by_path(new_enemy, path, new_val)

            enemies.append(new_enemy)
            # id はユニークに振り直し
            self._assign_unique_id_for_enemy(new_enemy, enemies, skip_index=len(enemies) - 1)

        self.refresh_enemy_list()

    def edit_raw_enemy(self):
        if self.current_stage_index is None or self.current_enemy_index is None:
            messagebox.showinfo("Info", "Enemy が選択されていません。")
            return
        self.save_current_enemy_detail()
        enemies = self.enemies_of_current_stage()
        e = enemies[self.current_enemy_index]
        RawEditorDialog(self, e, title="Edit Raw Enemy JSON")
        self._assign_unique_id_for_enemy(e, enemies, skip_index=self.current_enemy_index)
        self.refresh_enemy_list()

# =====================================
# Talk Editor
# =====================================

    # ---- helper: nested key access ----
    def _get_value_by_path(self, obj, path: str):
        """
        path: 'pos.x' や 'spawnTime' のようなドット区切りで
        ネストした値を取得する。
        """
        keys = path.split(".")
        cur = obj
        for k in keys:
            if isinstance(cur, dict) and k in cur:
                cur = cur[k]
            else:
                return None
        return cur

    def _set_value_by_path(self, obj, path: str, value):
        """
        path: 'pos.x' や 'spawnTime' のようなドット区切りで
        ネストした値を設定する。
        中間の dict が無ければ作る。
        """
        keys = path.split(".")
        cur = obj
        for k in keys[:-1]:
            if not isinstance(cur, dict):
                # dict じゃなかったら上書きして dict にする（通常は起きない想定）
                cur = {}
            cur = cur.setdefault(k, {})
        cur[keys[-1]] = value

    def bulk_add_enemy(self):
        if self.current_stage_index is None:
            return

        # 何個増やすか
        n = simpledialog.askinteger(
            "Bulk Add", "追加する Enemy 数を入力してください:",
            minvalue=1, maxvalue=1000
        )
        if not n:
            return

        enemies = self.enemies_of_current_stage()

        # テンプレート（選択中があればそれ、なければ DEFAULT_ENEMY）
        if self.current_enemy_index is not None and 0 <= self.current_enemy_index < len(enemies):
            template = enemies[self.current_enemy_index]
        else:
            template = DEFAULT_ENEMY

        # テンプレートから「数値のキー」のパス一覧を集める（例: 'time', 'spawnTime', 'pos.x', 'vel.y' など）
        def collect_numeric_paths(obj, prefix=""):
            paths = []
            if isinstance(obj, dict):
                for k, v in obj.items():
                    p = f"{prefix}.{k}" if prefix else k
                    if isinstance(v, (int, float)):
                        # id は自動で振り直しなので除外
                        if not (prefix == "" and k == "id"):
                            paths.append(p)
                    elif isinstance(v, dict):
                        paths.extend(collect_numeric_paths(v, p))
            return paths

        key_paths = collect_numeric_paths(template)
        if not key_paths:
            messagebox.showinfo("Bulk Add", "数値として扱えるキーが見つかりません。")
            return

        # ダイアログで「どのキーを等差的に変化させるか」とその増分を指定
        dlg = BulkAddDialog(self, key_paths)
        if dlg.result is None:
            # キャンセルされた
            return

        selected_steps = dlg.result  # {path: step(float), ...}

        # 何も選んでいない場合は単純クローン
        use_arith = bool(selected_steps)

        # 各キーごとに base 値と型情報を記録
        arith_info = {}
        if use_arith:
            for path, step in selected_steps.items():
                base_value = self._get_value_by_path(template, path)
                if not isinstance(base_value, (int, float)):
                    # collect_numeric_paths で数値に限定しているので通常ここには来ない想定
                    messagebox.showerror(
                        "Bulk Add",
                        f"キー '{path}' の元の値が数値ではありません: {base_value!r}",
                    )
                    return
                arith_info[path] = {
                    "base": base_value,
                    "step": step,
                    "is_int": isinstance(base_value, int),
                }

        # 実際の追加処理
        for i in range(n):
            # テンプレートのディープコピー
            new_enemy = json.loads(json.dumps(template))

            # 等差的な変更（複数キー対応）
            if use_arith:
                for path, info in arith_info.items():
                    new_val = info["base"] + info["step"] * (i + 1)
                    if info["is_int"]:
                        new_val = int(new_val)
                    self._set_value_by_path(new_enemy, path, new_val)

            enemies.append(new_enemy)
            # id はユニークに振り直し
            self._assign_unique_id_for_enemy(new_enemy, enemies, skip_index=len(enemies) - 1)

        self.refresh_enemy_list()
class TalkEditor(ttk.Frame):
    def __init__(self, master, app, *args, **kwargs):
        super().__init__(master, *args, **kwargs)
        self.app = app
        self.data = app.data  # {"stages":[{"talks":[...]},...]}
        self.current_stage_index = None
        self.current_talk_index = None

        self._build_ui()
        self.refresh_stage_list()

    # ---- flush ----
    def flush_changes(self):
        self.save_current_talk_detail()

    # ---- data ----
    @property
    def stages(self):
        return self.data.setdefault("stages", [])

    def talks_of_current_stage(self):
        if self.current_stage_index is None:
            return []
        st = self.stages[self.current_stage_index]
        return st.setdefault("talks", [])

    # ---- UI ----
    def _build_ui(self):
        left = ttk.Frame(self)
        left.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        ttk.Label(left, text="Talk Stage List").pack(anchor=tk.W)
        self.stage_listbox = tk.Listbox(left, height=10)
        self.stage_listbox.pack(fill=tk.BOTH, expand=True)
        self.stage_listbox.bind("<<ListboxSelect>>", self.on_stage_selected)

        sbf = ttk.Frame(left)
        sbf.pack(fill=tk.X, pady=4)
        ttk.Button(sbf, text="Add Stage", command=self.add_stage).pack(side=tk.LEFT, padx=2)
        ttk.Button(sbf, text="Apply", command=self.apply_stage).pack(side=tk.LEFT, padx=2)
        ttk.Button(sbf, text="Delete", command=self.delete_stage).pack(side=tk.LEFT, padx=2)

        right = ttk.Frame(self)
        right.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        tf = ttk.LabelFrame(right, text="Talks")
        tf.pack(fill=tk.BOTH, expand=True, pady=4)

        left_t = ttk.Frame(tf)
        left_t.pack(side=tk.LEFT, fill=tk.Y, padx=2, pady=2)

        ttk.Label(left_t, text="Talk List").pack(anchor=tk.W)
        self.talk_listbox = tk.Listbox(left_t, height=10)
        self.talk_listbox.pack(fill=tk.BOTH, expand=True)
        self.talk_listbox.bind("<<ListboxSelect>>", self.on_talk_selected)

        tbf = ttk.Frame(left_t)
        tbf.pack(fill=tk.X, pady=4)
        ttk.Button(tbf, text="Add", command=self.add_talk).pack(side=tk.LEFT, padx=2)
        ttk.Button(tbf, text="Apply", command=self.apply_talk).pack(side=tk.LEFT, padx=2)
        ttk.Button(tbf, text="Delete", command=self.delete_talk).pack(side=tk.LEFT, padx=2)
        ttk.Button(tbf, text="Clone", command=self.clone_talk).pack(side=tk.LEFT, padx=2)

        right_t = ttk.Frame(tf)
        right_t.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=2, pady=2)

        row = 0
        ttk.Label(right_t, text="talkStr (str)").grid(row=row, column=0, sticky=tk.W)
        self.talk_str_var = tk.StringVar()
        ttk.Entry(right_t, textvariable=self.talk_str_var, width=40).grid(row=row, column=1, sticky=tk.W)
        row += 1

        ttk.Label(right_t, text="Speaker").grid(row=row, column=0, sticky=tk.W)
        self.talk_is_enemy_var = tk.BooleanVar()
        ttk.Checkbutton(
            right_t, text="Enemy side (チェックで敵 / 外すと自機)", variable=self.talk_is_enemy_var
        ).grid(row=row, column=1, sticky=tk.W)
        row += 1

        raw_f = ttk.Frame(right)
        raw_f.pack(fill=tk.X, pady=4)
        ttk.Button(raw_f, text="Edit Raw JSON (Current Talk)", command=self.edit_raw_talk).pack(anchor=tk.W)

    # ---- Stage ----
    def refresh_stage_list(self):
        sel_index = self.current_stage_index if self.current_stage_index is not None else 0
        self.stage_listbox.delete(0, tk.END)
        for i, _ in enumerate(self.stages):
            self.stage_listbox.insert(tk.END, f"{i}: Stage{i+1}")
        if self.stages:
            if sel_index >= len(self.stages):
                sel_index = len(self.stages) - 1
            self.current_stage_index = sel_index
            self.stage_listbox.selection_set(sel_index)
        else:
            self.current_stage_index = None
            self.refresh_talk_list()

        self.show_current_stage_detail()

    def show_current_stage_detail(self):
        # TalkStage 自体には今のところ名前などの編集項目はないので、
        # 単に Talk リストを更新する。
        self.current_talk_index = None
        self.refresh_talk_list()

    def on_stage_selected(self, event=None):
        sel = self.stage_listbox.curselection()
        if not sel:
            return
        self.current_stage_index = sel[0]
        self.show_current_stage_detail()

    def add_stage(self):
        self.save_current_talk_detail()
        self.stages.append(json.loads(json.dumps(DEFAULT_TALK_STAGE)))
        self.current_stage_index = len(self.stages) - 1
        self.refresh_stage_list()

    def apply_stage(self):
        # 現状 TalkStage 側に編集項目はないのでリスト再描画のみ
        self.refresh_stage_list()

    def delete_stage(self):
        if self.current_stage_index is None:
            return
        if not messagebox.askyesno("Delete Talk Stage", "選択中 Stage を削除しますか？"):
            return
        self.stages.pop(self.current_stage_index)
        self.current_stage_index = None
        self.refresh_stage_list()

    # ---- Talk ----
    def refresh_talk_list(self):
        sel_index = self.current_talk_index if self.current_talk_index is not None else 0
        self.talk_listbox.delete(0, tk.END)
        talks = self.talks_of_current_stage()
        for i, t in enumerate(talks):
            s = t.get("talkStr", "")
            short = s[:10] + ("..." if len(s) > 10 else "")
            who = "E" if t.get("isTalkEnemy", 0) else "P"
            self.talk_listbox.insert(tk.END, f"{i}: [{who}] {short}")
        if talks:
            if sel_index >= len(talks):
                sel_index = len(talks) - 1
            self.current_talk_index = sel_index
            self.talk_listbox.selection_set(sel_index)
        else:
            self.current_talk_index = None
            self.clear_talk_detail()

        self.show_current_talk_detail()

    def clear_talk_detail(self):
        self.talk_str_var.set("")
        self.talk_is_enemy_var.set(False)

    def show_current_talk_detail(self):
        if self.current_stage_index is None or self.current_talk_index is None:
            self.clear_talk_detail()
            return
        talks = self.talks_of_current_stage()
        if not (0 <= self.current_talk_index < len(talks)):
            self.clear_talk_detail()
            return
        t = talks[self.current_talk_index]
        self.talk_str_var.set(t.get("talkStr", ""))
        self.talk_is_enemy_var.set(bool(t.get("isTalkEnemy", 0)))

    def save_current_talk_detail(self):
        if self.current_stage_index is None or self.current_talk_index is None:
            return
        talks = self.talks_of_current_stage()
        if not (0 <= self.current_talk_index < len(talks)):
            return
        t = talks[self.current_talk_index]
        t["talkStr"] = self.talk_str_var.get()
        t["isTalkEnemy"] = 1 if self.talk_is_enemy_var.get() else 0

    def on_talk_selected(self, event=None):
        sel = self.talk_listbox.curselection()
        if not sel:
            return
        self.current_talk_index = sel[0]
        self.show_current_talk_detail()

    def add_talk(self):
        if self.current_stage_index is None:
            return
        self.save_current_talk_detail()
        talks = self.talks_of_current_stage()
        talks.append(json.loads(json.dumps(DEFAULT_TALK)))
        self.current_talk_index = len(talks) - 1
        self.refresh_talk_list()

    def apply_talk(self):
        self.save_current_talk_detail()
        self.refresh_talk_list()

    def delete_talk(self):
        if self.current_stage_index is None or self.current_talk_index is None:
            return
        talks = self.talks_of_current_stage()
        if not (0 <= self.current_talk_index < len(talks)):
            return
        if not messagebox.askyesno("Delete Talk", "選択中 Talk を削除しますか？"):
            return
        talks.pop(self.current_talk_index)
        self.current_talk_index = None
        self.refresh_talk_list()

    def clone_talk(self):
        if self.current_stage_index is None or self.current_talk_index is None:
            return
        talks = self.talks_of_current_stage()
        src = talks[self.current_talk_index]
        talks.append(json.loads(json.dumps(src)))
        self.current_talk_index = len(talks) - 1
        self.refresh_talk_list()

    def edit_raw_talk(self):
        if self.current_stage_index is None or self.current_talk_index is None:
            messagebox.showinfo("Info", "Talk が選択されていません。")
            return
        self.save_current_talk_detail()
        talks = self.talks_of_current_stage()
        t = talks[self.current_talk_index]
        RawEditorDialog(self, t, title="Edit Raw Talk JSON")
        self.refresh_talk_list()


# =====================================
# メインアプリ
# =====================================

class JsonEditorApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("STG JSON Editor (Boss/Stage/Talk) - v4")

        self.scenario_var = tk.StringVar(value="story")
        self.category_var = tk.StringVar(value="Stage")  # Stage をデフォルトに

        self.root_dir = os.getcwd()
        self.data = {}  # 現在開いている JSON データ

        self.editor_frame = None

        self._build_ui()
        self.create_editor()

    # ---- UI ----
    def _build_ui(self):
        top = ttk.Frame(self)
        top.pack(fill=tk.X, padx=5, pady=5)

        ttk.Label(top, text="Scenario:").pack(side=tk.LEFT)
        ttk.OptionMenu(top, self.scenario_var, self.scenario_var.get(), "story", "extra").pack(side=tk.LEFT)

        ttk.Label(top, text="Category:").pack(side=tk.LEFT, padx=(10, 0))
        ttk.OptionMenu(top, self.category_var, self.category_var.get(), "Boss", "Stage", "Talk").pack(side=tk.LEFT)

        ttk.Button(top, text="Select Directory", command=self.select_directory).pack(side=tk.LEFT, padx=10)
        ttk.Button(top, text="Load", command=self.load_file).pack(side=tk.LEFT, padx=4)
        ttk.Button(top, text="Save", command=self.save_file).pack(side=tk.LEFT, padx=4)

        self.path_label_var = tk.StringVar(value=self.current_file_path())
        ttk.Label(top, textvariable=self.path_label_var).pack(side=tk.LEFT, padx=10)

        self.editor_container = ttk.Frame(self)
        self.editor_container.pack(fill=tk.BOTH, expand=True)

        self.scenario_var.trace_add("write", lambda *args: self.on_mode_changed())
        self.category_var.trace_add("write", lambda *args: self.on_mode_changed())

    # ---- file path ----
    def current_file_name(self):
        return f"{self.scenario_var.get()}{self.category_var.get()}.json"

    def current_file_path(self):
        return os.path.join(self.root_dir, self.current_file_name())

    # ---- editor ----
    def create_editor(self):
        if self.editor_frame is not None:
            self.editor_frame.destroy()
            self.editor_frame = None

        cat = self.category_var.get()
        if cat == "Boss":
            self.editor_frame = BossEditor(self.editor_container, self)
        elif cat == "Stage":
            self.editor_frame = StageEditor(self.editor_container, self)
        elif cat == "Talk":
            self.editor_frame = TalkEditor(self.editor_container, self)
        else:
            self.editor_frame = ttk.Frame(self.editor_container)
            ttk.Label(self.editor_frame, text="Unknown category").pack()

        self.editor_frame.pack(fill=tk.BOTH, expand=True)

    def on_mode_changed(self):
        # モード切り替え時は、現在の editor の内容は破棄して
        # 新しいカテゴリ用の空データから開始（Load で上書き）
        if self.editor_frame is not None and hasattr(self.editor_frame, "flush_changes"):
            self.editor_frame.flush_changes()
        self.data = {}
        self.path_label_var.set(self.current_file_path())
        self.create_editor()

    # ---- directory ----
    def select_directory(self):
        d = filedialog.askdirectory(initialdir=self.root_dir)
        if not d:
            return
        self.root_dir = d
        self.path_label_var.set(self.current_file_path())

    # ---- Load / Save ----
    def load_file(self):
        path = self.current_file_path()

        if not os.path.exists(path):
            messagebox.showinfo("Info", f"{path} が存在しません。新規作成として扱います。")
            cat = self.category_var.get()
            if cat == "Boss":
                self.data = {"bosses": []}
            elif cat == "Stage":
                self.data = {"stages": []}
            elif cat == "Talk":
                self.data = {"stages": []}
        else:
            try:
                with open(path, "r", encoding="utf-8") as f:
                    self.data = json.load(f)
            except Exception as e:
                messagebox.showerror("Error", f"JSON の読み込みに失敗しました:\n{e}")
                return

        self.create_editor()
        messagebox.showinfo("Loaded", f"Loaded: {path}")

    def save_file(self):
        if self.editor_frame is not None and hasattr(self.editor_frame, "flush_changes"):
            self.editor_frame.flush_changes()

        path = self.current_file_path()
        ensure_dir(self.root_dir)
        try:
            with open(path, "w", encoding="utf-8") as f:
                json.dump(self.data, f, ensure_ascii=False, indent=4)
        except Exception as e:
            messagebox.showerror("Error", f"JSON の保存に失敗しました:\n{e}")
            return

        self.path_label_var.set(path)
        messagebox.showinfo("Saved", f"Saved: {path}")


if __name__ == "__main__":
    app = JsonEditorApp()
    app.mainloop()
