# -*- coding: utf-8 -*-
# Task manager keeps the tasks created so far 
from __future__ import print_function

import os, shutil
import logging

class TaskManager(object):
    def __init__(self, task_prefix, local_folder, keep_intermediate_tasks):
        self._task_prefix = task_prefix
        self._task_counter = None
        self._local_folder = local_folder
        self._keep_intermediate_tasks = keep_intermediate_tasks

    def get_task_prefix(self):
        return self._task_prefix

    def get_task_counter(self):
        return self._task_counter

    def get_original_task_name(self):
        return "%s.0" % self._task_prefix

    def get_original_task_path(self):
        return os.path.join(self._local_folder, self.get_original_task_name())

    def get_current_task_name(self):
        if self._task_counter is None:
            return None
        return "%s.%s" % (self._task_prefix, self._task_counter)

    def get_current_task_path(self):
        name = self.get_current_task_name()
        if name is None:
            return None
        return os.path.join(self._local_folder, name)

    def add_task(self, task_name):
        task_path = os.path.join(self._local_folder, task_name)
        if not os.path.exists(task_path):
            logging.debug("No task exists in %s" % task_path)
            return False

        if not self._keep_intermediate_tasks and self._task_counter is not None and self._task_counter > 0:
            # deleting the current task
            current_task_path = self.get_current_task_path()
            if os.path.exists(current_task_path):
                os.remove(current_task_path)

        if self._task_counter is None:
            # adding original task
            self._task_counter = 0
        else:
            self._task_counter += 1
        os.rename(task_path, self.get_current_task_path())
        return True

    def copy_task(self, task_name):
        assert(self._task_counter is None)
        self._task_counter = 0
        dest = self.get_current_task_path()
        logging.debug("copying %s to %s" % (task_name, dest))
        shutil.copy2(task_name, dest)