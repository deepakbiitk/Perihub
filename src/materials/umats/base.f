      SUBROUTINE UMAT(STRESS,STATEV,DDSDDE,SSE,SPD,SCD,
     1 RPL,DDSDDT,DRPLDE,DRPLDT,
     2 STRAN,DSTRAN,TIME,DTIME,TEMP,DTEMP,PREDEF,DPRED,CMNAME,
     3 NDI,NSHR,NTENS,NSTATV,PROPS,NPROPS,COORDS,DROT,PNEWDT,
     4 CELENT,DFGRD0,DFGRD1,NOEL,NPT,LAYER,KSPT,JSTEP,KINC)
C
C    INCLUDE 'ABA_PARAM.INC'
      implicit real(8) (a-h,o-z)
C
      CHARACTER*80 CMNAME
      DOUBLE PRECISION, DIMENSION(6,6) :: C
      DOUBLE PRECISION, DIMENSION(NTENS) :: STRANNP1
      DIMENSION STRESS(NTENS),STATEV(NSTATV),
     1 DDSDDE(NTENS,NTENS),
     2 DDSDDT(NTENS),DRPLDE(NTENS),
     3 STRAN(NTENS),DSTRAN(NTENS),TIME(2),PREDEF(1),DPRED(1),
     4 PROPS(NPROPS),COORDS(3),DROT(3,3),DFGRD0(3,3),DFGRD1(3,3),
     5 JSTEP(4)
C
C  EVALUATE NEW STRESS TENSOR
C     
      DO K1=1,NTENS
            STRANNP1 = STRAN(K1) + DSTRAN(K1)
      END DO
      DO K1=1,NTENS
            DO K2=1,NTENS
                  C(K1,K2) = 0.0
            END DO
      END DO
      C(1,1) = PROPS(1)-PROPS(3)*PROPS(3)/PROPS(7)
      C(1,2) = PROPS(2)-PROPS(3)*PROPS(8)/PROPS(7)
      C(2,1) = PROPS(2)-PROPS(3)*PROPS(8)/PROPS(7)
      C(2,2) = PROPS(7)-PROPS(3)*PROPS(8)/PROPS(7)
      C(6,6) = PROPS(21)
      DO K1 =1,6
            STRESS(K1) = 0.D0
            DO K2 = 1, 6
                  STRESS(K1) = STRESS(K1) + C(K1,K2) *
     &            (STRAN(K2) + DSTRAN(K2))
            END DO
      END DO 
      RETURN
      END