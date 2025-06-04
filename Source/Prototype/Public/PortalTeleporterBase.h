// PortalTeleporterBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalTeleporterBase.generated.h" // ���� ������ ��� �̸����� ����

// ���� ����
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROTOTYPE_API APortalTeleporterBase : public AActor // 
{
    GENERATED_BODY()

public:
    // ������
    APortalTeleporterBase();

protected:
    // ���� ���� �Ǵ� ���� �� ȣ��
    virtual void BeginPlay() override;

public:
    // �÷��̾� ������ ���� Ʈ���� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components")
    UBoxComponent* TriggerVolume;

    // (���� ����) ��Ż�� �ð��� ǥ���� ���� ����ƽ �޽� ������Ʈ
    // �������Ʈ Ŭ���� ����Ʈ���� �޽ø� ������ �� �ֵ��� EditDefaultsOnly �߰�
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Portal Components")
    UStaticMeshComponent* PortalVisualMesh;

    // �̵��� ��ǥ ���� ��ǥ
    // �ν��Ͻ����� �����Ϳ��� ���� �����ϵ��� EditAnywhere ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport Settings", meta = (MakeEditWidget = true))
    FVector TargetWorldLocation; // ���� ��ǥ���� ���

    // ������ �߻� �� ȣ��� �Լ�
    UFUNCTION()
    void OnPortalOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};