<?php

namespace Drupal\penncourse\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Config\ConfigFactory;

/**
 * Class PenncourseController.
 */
class PenncourseController extends ControllerBase {

  /**
   * Drupal\Core\Config\ConfigFactory definition.
   *
   * @var \Drupal\Core\Config\ConfigFactory
   */
  protected $configFactory;

  /**
   * Constructs a new PenncourseController object.
   */
  public function __construct(ConfigFactory $config_factory) {
    $this->configFactory = $config_factory;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('config.factory')
    );
  }

  /**
   * Test.
   *
   * @return string
   *   Return Hello string.
   */
  public function test() {
    return [
      '#type' => 'markup',
      '#markup' => $this->t('Implement method: test')
    ];
  }

}
