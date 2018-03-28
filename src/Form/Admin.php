<?php

namespace Drupal\islandora_basic_image\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Module settings form.
 */
class Admin extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'islandora_basic_image_admin';
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('islandora_basic_image.settings');

    $config->set('islandora_basic_image_upscale_images', $form_state->getValue('islandora_basic_image_upscale_images'));

    $config->save();
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['islandora_basic_image.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['islandora_basic_image_upscale_images'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Upscale images for MEDIUM_SIZE derivatives?'),
      '#description' => $this->t('If selected will cause images that are smaller than 500x700 to be upscaled.'),
      '#default_value' => $this->config('islandora_basic_image.settings')->get('islandora_basic_image_upscale_images'),
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Submit'),
    ];
    return $form;
  }

}
