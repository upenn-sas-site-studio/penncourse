<?php

namespace Drupal\penncourse\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\penncourse\Service\PenncourseService;

/**
 * Class PenncourseFilterForm.
 */
class PenncourseFilterForm extends FormBase {

  /**
   * Drupal\penncourse\Service\PenncourseService definition.
   *
   * @var \Drupal\penncourse\Service\PenncourseService
   */
  protected $penncourseService;
  /**
   * Constructs a new PenncourseFilterForm object.
   */
  public function __construct(
    PenncourseService $penncourse_service
  ) {
    $this->penncourseService = $penncourse_service;
  }

  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('penncourse.service')
    );
  }


  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'penncourse_filter_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['term'] = [
      '#type' => 'select',
      '#title' => $this->t('Term'),
      '#options' => ['2018A' => $this->t('2018A'), '2018C' => $this->t('2018C')],
      '#size' => 1,
      '#weight' => '0',
    ];
    $form['subject'] = [
      '#type' => 'select',
      '#title' => $this->t('Subject'),
      '#options' => ['All department subjects' => $this->t('All department subjects'), 'ANTH' => $this->t('ANTH')],
      '#size' => 1,
      '#weight' => '0',
    ];
    $form['level'] = [
      '#type' => 'select',
      '#title' => $this->t('Level'),
      '#options' => ['All' => $this->t('All'), 'undergraduate' => $this->t('undergraduate'), 'graduate' => $this->t('graduate')],
      '#size' => 1,
      '#weight' => '0',
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Submit'),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    parent::validateForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // Display result.
    foreach ($form_state->getValues() as $key => $value) {
      drupal_set_message($key . ': ' . $value);
    }

  }

}
