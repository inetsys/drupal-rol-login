<?php

/**
 * @file
 * Definition of views_handler_field_user_link_edit.
 */

/**
 * Field handler to present a link to user edit.
 *
 * @ingroup views_field_handlers
 */
class rol_login_handler_field_users_edit_node extends views_handler_field_user_link {
	function render_link($data, $values) {
		$account = user_load($data);

		if($data && $account && rol_login_user_edit_access($account)){
			$this->options['alter']['make_link'] = TRUE;

			$text = !empty($this->options['text']) ? $this->options['text'] : t('edit');

			$this->options['alter']['path'] = 'user/'.$account->uid.'/edit';
			$this->options['alter']['query'] = drupal_get_destination();

			return $text;
		}
	}
}
